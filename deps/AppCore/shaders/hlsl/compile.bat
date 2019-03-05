fxc /T ps_4_0 /Fo fill.fxc ps/fill.hlsl /E PS
fxc /T ps_4_0 /Fo fill_path.fxc ps/fill_path.hlsl /E PS
fxc /T vs_4_0 /Fo v2f_c4f_t2f.fxc vs/v2f_c4f_t2f.hlsl /E VS
fxc /T vs_4_0 /Fo v2f_c4f_t2f_t2f_d28f.fxc vs/v2f_c4f_t2f_t2f_d28f.hlsl /E VS
bin2header fill.fxc 
bin2header fill_path.fxc
bin2header v2f_c4f_t2f.fxc
bin2header v2f_c4f_t2f_t2f_d28f.fxc
del *.fxc