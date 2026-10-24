# 3dss-thermal-scopes-fix
In default 3DSS, all thermal vision modes share the same hardcoded values, 
which makes tweaking one mode mess up the others.
I rewrote the shader logic to split everything into three independent sets of 
values: Dark Mode, Light Mode, and Color Mode.

Now, each mode has its own dedicated settings for cold backgrounds, 
enemy hot spots, skybox brightness, and an overall brightness multiplier,
so you can tune White Hot, Black Hot, and Color thermal modes
completely independently without compromises.

I also remade the T12 scope reticle image to strip out the blue tint,
it was interfering with the shader rendering and causing problems.


If you want to tweak the values yourself, go to:
..\Gamma\mods\3dss thermal scopes fix unofficial\gamedata\shaders\r3\thermal_utils.h

I left comments in the file explaining what each value does,
so you know what to tweak.

Installation:
    
  1.  Delete your shader cache (found in ..\Anomaly\appdata)
  2. Place this mod at the bottom of your MO2 load order so it overwrites 3DSS 5.x.
  3. Don't forget to delete your shader cache!


Hope you enjoy.

(I used AI to help me learn how to code this.)


Note: If anyone knows whether it's possible to hook these thermal image settings 
into MCM (Mod Configuration Menu), please let me know.
<br><br><br>


<img width="50%" alt="00 monochrome" src="https://github.com/user-attachments/assets/e93ef05e-734e-4c93-9670-a9e429e39b10" />

<br><br><br><br><br><br>

<img width="50%" alt="01 color" src="https://github.com/user-attachments/assets/8a803bde-380f-4390-86d9-ae13af15b477" />

