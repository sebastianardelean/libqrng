#!/bin/bash
echo "Build script created by Sebastian M. Ardelean <sebastian.ardelean@cs.upt.ro>"
echo -e "Start deployment[$(date +"%T")]...\n"

#GLOBAL_VARIABLES
SW_NAME="qrand"           #archive name
BUILD_DATE=$(date +%d.%m.%Y)    #build date
ARCH="amd64"
TYPE="no-dynamic-malloc"
WORKING_DIRECTORY=$(pwd)        #save working directory
MAJOR_VERSION=0
MINOR_VERSION=0
BUILD_NUMBER=0
ERROR_NUMBER=0;


#FUNCTIONS
function compile_project() {            #function will accept 1 parameter and will be called compile_project "parameter"
    echo -e "\nCompiling $1...[$(date +"%T")]\n"
    path=$1                             #$1 is the directory of the source code that will be compiled
    cd $path                            #change directory to the provided path
    make                                #first clean and then make the project
    ERROR_NUMBER=$(echo $?)             #get error number
    if [ $ERROR_NUMBER -ne 0 ]; then
        exit $ERROR_CODE
    fi
    cd $WORKING_DIRECTORY               #change back to the working directory
    }

function clean_project() {              #function will accept 1 parameter and will be called compile_project "parameter"
    echo -e "\nCleaning $1...[$(date +"%T")]\n"
    path=$1
    cd $path
    make clean
    ERROR_NUMBER=$(echo $?)             #get error number
    if [ $ERROR_NUMBER -ne 0 ]; then
        exit $ERROR_CODE
    fi
    cd $WORKING_DIRECTORY
}

function compile_install_library() {            #function will accept 1 parameter and will be called compile_project "parameter"
    echo -e "\nCompiling $1...[$(date +"%T")]\n"
    path=$1                             #$1 is the directory of the source code that will be compiled
    cd $path                            #change directory to the provided path
    make FLAGS="$2"                     #first clean and then make the project
    make install
    ERROR_NUMBER=$(echo $?)             #get error number
    if [ $ERROR_NUMBER -ne 0 ]; then
        exit $ERROR_CODE
    fi
    cd $WORKING_DIRECTORY               #change back to the working directory
    }


function extract_version() {                                    #function has no parameter
    versionFile=$(ls ../examples/qrand/ | grep version.*)            #list qrand directory and send result to grep to extract the file version.hpp
    
    #grep previous values from version.hpp
    MAJOR_VERSION=$(grep -Eo 'MAJOR_VERSION\s([0-9]{1,3})' ../examples/qrand/$versionFile | grep -Eo '[0-9]{1,3}')
    ERROR_NUMBER=$(echo $?)             #get error number
    if [ $ERROR_NUMBER -ne 0 ]; then
        exit $ERROR_CODE
    fi
    
    MINOR_VERSION=$(grep -Eo 'MINOR_VERSION\s([0-9]{1,3})' ../examples/qrand/$versionFile | grep -Eo '[0-9]{1,3}')
    ERROR_NUMBER=$(echo $?)             #get error number
    if [ $ERROR_NUMBER -ne 0 ]; then
        exit $ERROR_CODE
    fi
    BUILD_NUMBER=$(grep -Eo 'BUILD_NUMBER\s([0-9]{1,3})' ../examples/qrand/$versionFile | grep -Eo '[0-9]{1,3}')
    ERROR_NUMBER=$(echo $?)             #get error number
    if [ $ERROR_NUMBER -ne 0 ]; then
        exit $ERROR_CODE
    fi
    
    #increment version logic
    BUILD_NUMBER=`expr $BUILD_NUMBER + 1`
    if [ $BUILD_NUMBER -eq 256 ]; then
        MINOR_VERSION=`expr $MINOR_VERSION + 1`
        BUILD_NUMBER=0
        
    fi
    
    if [ $MINOR_VERSION -eq 256 ]; then
        MINOR_VERSION=0
        MAJOR_VERSION=`expr $MAJOR_VERSION + 1`
    fi
    
    if [ $MAJOR_VERSION -eq 256 ]; then
        MAJOR_VERSION=0
    fi
    
}

function set_new_version_number() {                                             #function has no parameters
    versionFile=$(ls ../examples/qrand/ | grep version.*)                                    #list qrand directory and send result to grep to extract the file version.hpp
    sed -i "s/.*MAJOR_VERSION.*/#define MAJOR_VERSION ${MAJOR_VERSION}/" ../examples/qrand/$versionFile
    sed -i "s/.*MINOR_VERSION.*/#define MINOR_VERSION ${MINOR_VERSION}/" ../examples/qrand/$versionFile
    sed -i "s/.*BUILD_NUMBER.*/#define BUILD_NUMBER ${BUILD_NUMBER}/" ../examples/qrand/$versionFile
    sed -i "s/.*BUILD_DATE.*/#define BUILD_DATE \"${BUILD_DATE}\"/" ../examples/qrand/$versionFile
    }




function create_archive() {
    ###############################
    # Start Building Executable   #
    # with memory allocation      #
    ###############################

    #create buffer directory for saving .so files
    TYPE=$1
    FOLDER_NAME=$SW_NAME-$MAJOR_VERSION.$MINOR_VERSION.$BUILD_NUMBER-$BUILD_DATE\_$ARCH-$TYPE
    echo -e "\nCreating directory $FOLDER_NAME![$(date +"%T")]\n"
    mkdir $WORKING_DIRECTORY/$SW_NAME-$MAJOR_VERSION.$MINOR_VERSION.$BUILD_NUMBER-$BUILD_DATE\_$ARCH-$TYPE


    # Clean library
    clean_project ../src

    #Compile library
    compile_install_library ../src $2
    cp ../lib/libqrng.so.1.0 $WORKING_DIRECTORY/$SW_NAME-$MAJOR_VERSION.$MINOR_VERSION.$BUILD_NUMBER-$BUILD_DATE\_$ARCH-$TYPE


    # Clean project
    clean_project ../examples/qrand

    #Compile Executable

    compile_project ../examples/qrand
    cp ../bin/qrand $WORKING_DIRECTORY/$SW_NAME-$MAJOR_VERSION.$MINOR_VERSION.$BUILD_NUMBER-$BUILD_DATE\_$ARCH-$TYPE



    ###############################
    # Create Install Script       #
    ###############################
    echo -e "\nCreating Install SHELL Script...[$(date +"%T")]\n"
    cd $WORKING_DIRECTORY/$SW_NAME-$MAJOR_VERSION.$MINOR_VERSION.$BUILD_NUMBER-$BUILD_DATE\_$ARCH-$TYPE
    touch install.sh

    echo "#!/bin/bash" >> install.sh
    echo "cp  *.so.* /usr/lib" >> install.sh
    echo "ln -sf /usr/lib/libqrng.so.1.0 /usr/lib/libqrng.so" >> install.sh
    echo "ldconfig" >> install.sh


    cd $WORKING_DIRECTORY

    ###############################
    # Start Creating Archive      #
    ###############################
    echo -e "Creating deploy archive...[$(date +"%T")]\n"
    tar -zcvf $SW_NAME-$MAJOR_VERSION.$MINOR_VERSION.$BUILD_NUMBER-$BUILD_DATE\_$ARCH-$TYPE.tar.gz $SW_NAME-$MAJOR_VERSION.$MINOR_VERSION.$BUILD_NUMBER-$BUILD_DATE\_$ARCH-$TYPE


    ###############################
    # Start Creating Archive      #
    ###############################


    echo -e "Clean install.sh from buffer folder...[$(date +"%T")]\n"
    rm $WORKING_DIRECTORY/$SW_NAME-$MAJOR_VERSION.$MINOR_VERSION.$BUILD_NUMBER-$BUILD_DATE\_$ARCH-$TYPE/install.sh


    echo -e "Create usr/local/bin for executable...[$(date +"%T")]\n"
    mkdir -p $WORKING_DIRECTORY/$SW_NAME-$MAJOR_VERSION.$MINOR_VERSION.$BUILD_NUMBER-$BUILD_DATE\_$ARCH-$TYPE/usr/local/bin

    echo -e "Copy executable to usr/local/bin...[$(date +"%T")]\n"
    mv $WORKING_DIRECTORY/$SW_NAME-$MAJOR_VERSION.$MINOR_VERSION.$BUILD_NUMBER-$BUILD_DATE\_$ARCH-$TYPE/qrand $WORKING_DIRECTORY/$SW_NAME-$MAJOR_VERSION.$MINOR_VERSION.$BUILD_NUMBER-$BUILD_DATE\_$ARCH-$TYPE/usr/local/bin


    echo -e "Create usr/local/lib for library...[$(date +"%T")]\n"
    mkdir -p $WORKING_DIRECTORY/$SW_NAME-$MAJOR_VERSION.$MINOR_VERSION.$BUILD_NUMBER-$BUILD_DATE\_$ARCH-$TYPE/usr/local/lib

    echo -e "Copy executable to usr/local/bin...[$(date +"%T")]\n"
    mv $WORKING_DIRECTORY/$SW_NAME-$MAJOR_VERSION.$MINOR_VERSION.$BUILD_NUMBER-$BUILD_DATE\_$ARCH-$TYPE/libqrng.so.1.0 $WORKING_DIRECTORY/$SW_NAME-$MAJOR_VERSION.$MINOR_VERSION.$BUILD_NUMBER-$BUILD_DATE\_$ARCH-$TYPE/usr/local/lib/libqrng.so


    echo -e "Create usr/local/include for library...[$(date +"%T")]\n"
    mkdir -p $WORKING_DIRECTORY/$SW_NAME-$MAJOR_VERSION.$MINOR_VERSION.$BUILD_NUMBER-$BUILD_DATE\_$ARCH-$TYPE/usr/local/include

    echo -e "Copy include to usr/local/include...[$(date +"%T")]\n"
    cp ../src/qrng.h $WORKING_DIRECTORY/$SW_NAME-$MAJOR_VERSION.$MINOR_VERSION.$BUILD_NUMBER-$BUILD_DATE\_$ARCH-$TYPE/usr/local/include

    ###############################
    # Create control file         #
    ###############################
    echo -e "\nCreating file...$(date +"%T")]\n"
    mkdir -p $WORKING_DIRECTORY/$SW_NAME-$MAJOR_VERSION.$MINOR_VERSION.$BUILD_NUMBER-$BUILD_DATE\_$ARCH-$TYPE/DEBIAN
    cd $WORKING_DIRECTORY/$SW_NAME-$MAJOR_VERSION.$MINOR_VERSION.$BUILD_NUMBER-$BUILD_DATE\_$ARCH-$TYPE/DEBIAN
    touch control



    echo "Package: qrand" >> control
    echo "Version:" $MAJOR_VERSION.$MINOR_VERSION.$BUILD_NUMBER >> control
    echo "Architecture:" $ARCH >> control
    echo "Maintainer: Sebastian M. Ardelean <sebastian.ardelean@cs.upt.ro>" >> control
    echo "Depends: libcurl3-gnutls" >> control
    echo "Description: Application to generate quantum random numbers" >> control
    echo " qrand is a command line application for generating random numbers using IDQ Quantis Appliance" >> control
    echo -e "\n" >> control

    cd $WORKING_DIRECTORY

    dpkg-deb --build --root-owner-group $SW_NAME-$MAJOR_VERSION.$MINOR_VERSION.$BUILD_NUMBER-$BUILD_DATE\_$ARCH-$TYPE

    echo -e "Cleaning...[$(date +"%T")]\n"
    # delete directory used for creating archive.tar.gz
    rm -r $SW_NAME-$MAJOR_VERSION.$MINOR_VERSION.$BUILD_NUMBER-$BUILD_DATE\_$ARCH-$TYPE
    # clean obj and so files from projects
    clean_project ../examples/qrand
    clean_project ../src
}


###############################
# PREPARING VERSION NUMBER    #
###############################
echo -e "\nExtracting version number![$(date +"%T")]\n"
extract_version


echo -e "\nSetting version number![$(date +"%T")]\n"
set_new_version_number


create_archive "no-malloc" "-DNO_DYNAMIC_MEMORY_ALLOCATION"

create_archive "malloc" ""

echo -e "Finished deployment![$(date +"%T")]\n"


###############################
# Building examples           #
###############################
compile_project ../examples/qrand

compile_project ../examples/fwversion

compile_project ../examples/sysinfo
