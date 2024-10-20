@rem windows compile of Easy Image Viewer using msvc 14.

cl /Feiview.exe -DSFKPIC -DSFKMINCORE -Isrc src\iview.cpp src\sfk.cpp src\sfkext.cpp src\sfkpic.cpp src\iviewres.res kernel32.lib user32.lib gdi32.lib shell32.lib comdlg32.lib advapi32.lib /link /SUBSYSTEM:WINDOWS

