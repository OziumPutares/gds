# GDS Login page
## Installing
### Dependencies: 
* [VCPKG](https://vcpkg.io/en/)
* [Cmake](https://cmake.org/)
* A C++23 compiler e.g. [GCC14](https://gcc.gnu.org/) or [Clang 19](https://clang.llvm.org/)
* A build tool supported by CMake e.g. [Ninja](https://ninja-build.org/)
### Installing dependencies on linux:
* Archlinux 
    ```
    sudo pacman -Syu base-devel vcpkg cmake gcc drogon
    git clone https://github.com/microsoft/vcpkg.git $VCPKG_ROOT
    ```
* Debian
    ```
    sudo apt-get update
    sudo apt-get install gcc ninja cmake make drogon
    ```
    Then follow [vcpkg install guide](https://learn.microsoft.com/en-gb/vcpkg/get_started/get-started?pivots=shell-bash)
### Installing on windows
Find each dependency and install according 
I couldn't be asked as no universal package manager 
### MacOs
Same thing as for windows
### Building
Run these commands
```
    cmake --preset=vcpkg
    cmake --build build
```
The executable will be found under ./build/Debug/GDSBackend
The webiste is found on port 8080 or http://localhost:8080/
## Market Research
* Upon surveying 30 people I found that 63.3% Percent of people thought they would like a central login page for GOV.UK. 
* Upon interviewing my computing teacher I also found the main annoyance of using GOV.UK was having to repeatedly login to access services releated to their child.
* Furthermore after asking the chatbot I found out that the main annoyance for users is having to repeatedly enter their data for multiple services.
* There have currently been not studies looking into how to improvve GOV.UK hence no data to be found
* Other Countries I found out that estonia uses a service for all government related services. Furthermore use their ID cards for authentication, signing document and even voting
## Problem Curation 
I found out the main reasons that there currently is no unified login system for all government services is due to the disjointed nature of government services which is partly due to the nature of government and privacy concerns. Hence  my solution has to maintain user privacy (through encrpytion) and try to allow for the easy transfer of data through OpenPGP signing of documents. 
## Solution
As hinted many times before the soluion will involve a sign in/login page that stores each users data in a server (soon to be encrypted). This is done through the use of drogon and vanilla JS, guaranteeing maximum performance for any client (that doesn't have 1Tb of ram to be eaten up by react webistes) and guaranteeing that minimal resources are required to host the website.  