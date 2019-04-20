
# Install Visual Studio
1.	Please use the 2015 version of Visual Studio. You can find the setup on the ftp server: ftp://ftp.sharif.ir/Engineering/Visual.Studio/Visual.Studio.Enterprise.2015/
2.	Use Custom installation
3.	Make sure you check the box next to Visual C++, GitHub integration, and Git for Windows
4.	When the installation is complete, launch Visual Studio, and select the Visual C++ as your environment


# Install Qt
1.	Point to the Qt website (qt.io)
2.	Download the Qt 5.8.0 offline installer for Visual Studio; note to choose the correct architecture for your system between 32 or 64 bit (64 bit is highly recommended)
3.	Launch the installer and follow the instructions to complete the installation
4.	***Restart you computer***

# Integrate Qt in Visual Studio
1.	In Visual Studio, point to Tools > Extension and Updates > Online
2.	Search for Qt
3.	Click on “Qt Visual Studio Tools,” click on Download
4.	When the download is over, click on Install
5.	Now a menu bar item named “Qt VS Tools” must have been added to Visual Studio
6.	Point to Qt VS Tools > Qt Options
7.	Click on “Add”
8.	Enter the Qt version you have installed in the “Version name,” e.g., 5.8.0-x64 or 5.8.0-x86
9.	Browse for Path; locate the “bin” folder in the Qt installation folder and click on one folder up, e.g., on msvc2015
10.	Click on OK buttons
11.	You may repeat this procedure to add all installed Qt versions



# **Admin** – Setup the GitLab Account and the Respective Branch

Creating the GitLab Account
1.	Head to devrtx.civil.sharif.edu
2.	Click on the Admin Area button at the top of the page ( )
3.	Click on Users
4.	Click on New User ( )
5.	Fill the form accordingly
6.	Click on Create User
7.	An email will be sent to the user to activate her/his account with their own password

Creating Access to the GitLab Repository
1.	Head to devrtx.civil.sharif.edu
2.	Click on Rt_Dev/rtrepo
3.	Click on Members
4.	In the Add member section select the user and choose Developer as role permission
5.	Click on Add to project

Adding the Personal Branch
1.	Head to devrtx.civil.sharif.edu
2.	Click on Rt_Dev/rtrepo
3.	Click on Branches
4.	Click on New Branch
5.	Enter the Developer’s First and Last Name (without spaces) as the branch name
6.	Click on Create Branch

Use this address to add a new user (add a ‘Regular’ type for students): http://devrtx.civil.sharif.edu/admin/users/new
Use this link to add the new member to the repository – use ‘Developer’ role for researchers, so they will not be able to modify the master branch: 
http://devrtx.civil.sharif.ir/Rt_Dev/rtrepo/settings/members



![teaser](http://i.imgur.com/1XENIUB.png)



# Setup the Rt Repository in Visual Studio

1.	Ask the Administrator to provide you with a Gitlab account and a development branch.
2.	In Visual Studio go to Team -> Manage Connections
3.	Click on Clone
4.	Input your branch address as follow:
Your branch address:  
http://yourbranchname@devrtx.civil.sharif.edu/Rt_Dev/rtrepo
Master branch address:
http://devrtx.civil.sharif.edu/Rt_Dev/rtrepo
5.	 Choose the location on your PC where you want to save the repository
6.	 Click on the Clone button
7.	 You will be asked for your credentials. Use your Gitlab account’s username and password. 

To switch to your allocated branch:
1.	In Visual Studio go to View -> Team Explorer
2.	At the top of the Team Explorer pane select the Home button
3.	Select Branches
4.	Open the remote/origin tree
5.	Right click on the item with your name and select Checkout

![teaser](http://i.imgur.com/H8gp4Kd.png)

![teaser](https://i.imgur.com/2BOJsTh.png)

# Pull Rt from Remote Repository
1.	In Visual Studio go to View -> Team Explorer
2.	At the top of the Team Explorer pane select the Home button
3.	Click on Sync
4.	Click on Pull 
A faster alternative:
1.	Click on the upward pointing arrow (  )at the bottom-right of your window 
2.	Click on Pull 
If you intend to check if there is any new changes, you can use the ‘Fetch’ option. By using ‘Fetch’ you will be informed of the changes without unintentionally pulling the remote repository.

![teaser](https://i.imgur.com/HYgwgSr.png)

# Compile Rt
1.	Open Visual Studio
2.	Select the desired platform, e.g., x86 or x64
3.	Select the build type from either Debug or Release. Debug builds are slower but can provide valuable information and features, e.g. breakpoints and debugging analysis during development. Release builds are faster but have limited features for debugging the program. 
4.	It is highly recommended to use the Debug-x64 build (Figure 1)
5.	Run the GSL.bat file in the Rtx folder
6.	Point to Build -> Build Solution
7.	Wait until Rt is compiled (It can take up to ten minutes to compile Rtx for the first time
8.	You will find the generated executable of Rtx in a subfolder of the Bin folder that depends on the platform, e.g., x64, and configuration, e.g., Release


# Commit and Push to Remote Repository
If your version of repository is outdated, you will be noticed to pull the latest version (like the image below). You will be able to push your changes to your desired branch after pulling the remote repository.
![teaser](https://i.imgur.com/3gaAW6U.png)
Committing Changes

If you want to include all of the changes that you have made in your commits follow these steps:
1.	In Visual Studio go to View -> Team Explorer
2.	At the top of the Team Explorer pane select the Home button
3.	Click on Change
4.	Enter the detail of your commit as the commit message (Required)
5.	Click on Commit All 

If you want to include some of the changes that you have made in your commits follow these steps:
1.	In Visual Studio go to View -> Team Explorer
2.	At the top of the Team Explorer pane select the Home button
3.	Click on Change
4.	Enter the detail of your commit as the commit message (Required)
5.	Select the changes that you want to commit from the tree view
6.	Right-click and select Stage
7.	Click on Commit Staged 

Pushing to Remote Repository
1.	In Visual Studio go to View -> Team Explorer
2.	At the top of the Team Explorer pane select the Home button
3.	Click on Sync
4.	Click on Push 

Resetting Commits
If you need to reset to an older commit, follow these steps:
1.	In Visual Studio click on the branch name placed at the right bottom corner
2.	Click on View History
3.	A list of the previous and recent commits shows up. Right-Click on the commit you want to restore
4.	Click on Reset
5.	If you want to remove the changes made after your intended commit, click on Reset and Delete Changes (--hard). If you just want to modify the changes and recommit select (--mixed)

IMPORTANT: When selecting the Reset and Delete option, newer commits will be removed from the repo and you will not be able to restore them if you do not have a backup or have not pushed the changes to the server. So use this option with care.

[doc-image]: http://i.stack.imgur.com/erf8e.png
[dev-image]: http://i.stack.imgur.com/D9G2G.png
[issues-image]: http://i.stack.imgur.com/K4fGd.png
[contact-image]: http://i.stack.imgur.com/tCbmW.png
