#Output file that MNSIS should write the installer to. The file that MNSIS writes
outFile "MarcoInstaller.exe"
installDir "$PROGRAMFILES\Jump Marco"

#Name of the installer
Name "Jump Marco, Jump"
#Window title
Caption "Installing 'Jump Marco, Jump'"

#Page command allows user to configure the installer on their own
Page directory       #User can choose where to save the game
Page instfiles       #Installation progress display

section "Install"
        SetOutPath "$INSTDIR"       #Path chosen by the user
        
        # the /r command means recursive, will add all of the files and subdir inside the specified folder
        #Copying all the necessary files
        File /r "C:\https-github.com-noemifavilli-jumpMarco\jump_marco\*"
        File /r "C:\https-github.com-noemifavilli-jumpMarco\lib\*"
        File /r "C:\https-github.com-noemifavilli-jumpMarco\res\*"
        
        #Create shortcut for desktop
        #Create a new shortcut in the start menu programs directory: first set of ""
        #Point the shortcut to the program unistaller: second set of ""
        CreateShortcut "$DESKTOP\JumpMarco.lnk" "$INSTDIR\marco.exe" #Find marco.exe path and put it here
        #Create an uninstaller shortcut too
        CreateShortcut "$PROGRAMFILES\Jump Marco, Jump\Uninstall Jump Marco.lmk" "$INSTDIR\uninstall.exe"
        #Generate uninstaller file
        WriteUninstaller "$INSTDIR\uninstall.exe"
sectionend

#For the uninstaller, this section is always called like this
section "Uninstall"
        #Delete game exe
        Delete "$INSTDIR\marco.exe"
        #Delete shortcut
        Delete "$DESKTOP\JumpMarco.lnk"
        #Delete uninstaller shortcut
        Delete "$PROGRAMFILES\Jump Marco, Jump\Uninstall Jump Marco.lnk"
        #Remove installation dir and contents
        RMDir /r "$INSTDIR"
sectionend

#Display to user uninstallation progress
UninstPage instfiles