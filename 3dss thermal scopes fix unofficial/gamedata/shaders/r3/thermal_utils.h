/*
	=====================================================================
	Original Author  : vegeta1k95
	Link             : https://www.moddb.com/mods/stalker-anomaly/addons/heatvision-v02-extension-for-beefs-nvg-dx11engine-mod
	=====================================================================
*/

#include "common.h"

#ifdef USE_MSAA
	#ifndef SM_5
        Texture2DMS<float4,MSAA_SAMPLES> s_heat;
	#else
        Texture2DMS<float4> s_heat;
	#endif
#else
    Texture2D   s_heat;
#endif
//                   Thermal image settings
/*

                            WARNING: 
DO NOT SET BRIGHTNESS TO 0 OR 1 KEEP IT STRICTLY IN BETWEEN THEM!!!


TECHNICAL NOTE: Brightness is applied via direct decimal multiplication. 
Conditional checks for edge values are intentionally omitted to avoid 
GPU branching overhead and optimize shader instruction count.

*/
//                          for dark mode 
//              (default, if the others are not used)
#define DARK_COLD_MIN    0.1    // the darkest cold areas can reach    (lower = darker)
#define DARK_COLD_MAX    0.2    // the brightest cold areas can reach  (lower = darker)
#define DARK_HOT         0.8    // enemies brightness                  (lower = darker)
#define DARK_SKY         0      // skybox brightness                   (lower = darker)
#define DARK_BRIGHTNESS  0.5    // overall brightness                  (lower = darker)

//                      for white mode
//                   used if "switchMode = 1"
#define LIGHT_COLD_MIN   0.7    // the darkest cold areas can reach    (lower = darker)
#define LIGHT_COLD_MAX   1      // the brightest cold areas can reach  (lower = darker)
#define LIGHT_HOT        0.01   // enemies brightness                  (lower = darker)
#define LIGHT_SKY        1      // skybox brightness                   (lower = darker)
#define LIGHT_BRIGHTNESS 0.6    // overall brightness                  (lower = darker)

//                      for color mode
//                   used if "imageType == 3"
#define COLOR_COLD_MIN   0.1    // the darkest cold areas can reach    (lower = darker) 
#define COLOR_COLD_MAX   0.3    // the brightest cold areas can reach  (lower = darker) 
#define COLOR_HOT        0.94   // enemies brightness                  (lower = darker)
#define COLOR_SKY        0.01   // skybox brightness                   (lower = darker)
#define COLOR_BRIGHTNESS 0.6    // overall brightness                  (lower = darker)
	
float3 normal_blur(float2 pos2d, int samples) 
{
    float3 accum = (0.0, 0.0, 0.0);

    for (int i = -samples; i < samples; i++) {        
        for (int j = -samples; j < samples; j++) {
    		accum += gbuf_unpack_normal(s_position.Load(int3(pos2d + float2(i, j), 0), 0).xy);
        }
	}
    
    return normalize(accum);
}

/* gets information from "models_scope_reticle.ps" and/or "models_scope_reticle_precise.ps" */
/* i added 2 additional arguments: int imageType = 0, int switchMode = 0, to know if the function should use the settings for them*/
/* if it gets only 3 arguments, like how 3dss is by default, it will use dark mode*/
float infrared(gbuffer_data gbd, float2 HPos, float2 Tex0, int imageType = 0, int switchMode = 0)
{	
    float3 hotness3 = s_heat.Load(int3(Tex0 * screen_res.xy, 0 ), 0).rgb;
	float hotness = max(hotness3.r, max(hotness3.g, hotness3.b));
    

    /* uses dark mode settings by deafult, and replaces them when needed*/
        float cold_min       = DARK_COLD_MIN;
        float cold_max       = DARK_COLD_MAX;
        float hot_val        = DARK_HOT;
        float sky_val        = DARK_SKY;
        float val_multiplier = DARK_BRIGHTNESS;

    /* checks if it uses color mode, then it replaces dark mode with color mode settings */
    /* it inverts the values because it doesnt look right otherwise, i dont really undertand why */
    if (imageType == 3) {
        cold_min       = COLOR_COLD_MAX; /* these needed to be reversed. current value logic is good now */
        cold_max       = COLOR_COLD_MIN; /* these needed to be reversed. current value logic is good now */
        hot_val        = COLOR_HOT;
        sky_val        = COLOR_SKY;
        val_multiplier = COLOR_BRIGHTNESS;
    }
    /* checks if it light color mode, then it replaces dark mode with light mode settings  */
    else if (switchMode == 1){
        cold_min       = LIGHT_COLD_MIN;
        cold_max       = LIGHT_COLD_MAX;
        hot_val        = LIGHT_HOT;
        sky_val        = LIGHT_SKY;
        val_multiplier = LIGHT_BRIGHTNESS;
    }

    float depth = gbd.P.z;
	if (depth <= SKY_EPS)
	{
		return sky_val;
	}
	
	float hot_color = cold_max;
    if (hotness > 0.0)
    {
        int samples = lerp(15, 4, smoothstep(0.0, 60, depth));
        float3 normal = normal_blur(HPos, samples);
        float projection = dot(normal, float3(0.0, 0.0, -1.0));
        
        if (projection <= 0.f) {
            hot_color = cold_max;
        } else {        
            hot_color = lerp(cold_min, hot_val, smoothstep(0, 1.0, projection));
        }
	}
	
	float projection = dot(normalize(gbd.N), float3(0.0, 0.0, -1.0));
	float mixed = lerp(cold_min, cold_max, projection);

    float result = lerp(mixed, hot_color, hotness);
    result *= val_multiplier; /* this is the final brightness, i multiply with 0.x cuz i thought it was easiest this way */

    return result;
}