if isunix % Building on Linux     
    mex('-v','src/sdl_joystick.c','-lSDL'); 
else % Building on Windows     
    %mex('-v','src/sdl_joystick.c','-I../src/windows/SDL2-2.0.1/include','-L../src/windows/SDL2-2.0.1/lib/x86','-lSDL2');    
    mex('src/sdl_joystick.c','-I../src/windows/SDL2-2.0.1/include','-L../src/windows/SDL2-2.0.1/lib/x86','-lSDL2');    
    %copyfile(['SDL/bin/' computer('arch') '/SDL.dll'],'.');
    copyfile('../src/windows/SDL2-2.0.1/lib/x86/SDL2.dll', '.');     
end 
fprintf('Build done!\n');