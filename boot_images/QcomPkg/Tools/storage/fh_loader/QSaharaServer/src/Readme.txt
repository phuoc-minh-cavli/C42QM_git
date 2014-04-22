Building for Windows:-

#Ensure you have Visual Studio 2017 installed.
#In Visual Studio, create a New Project using File->New->Project from Existing code.
#Add "/src" folder to the project.
#Execute from menu bar Build->Build Solution (if building for the first time) or Build->Rebuild Solution.
#Please enable /MT flag in the Runtime Library settings for the executable to work on all Windows machines

== Building for Linux ==

#Ensure you have Cmake application installed on your Linux system.
#Open terminal inside the "/src" folder.
#Run "cmake CmakeLists.txt" or <b>"cmake ."</b> on the terminal. 
#Then run "make" to generate the QSaharaServer executable. 
