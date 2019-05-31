###############################################################################
#
#       Copyright (c) 2008-2015 Qualcomm Technologies International, Ltd.
#       All Rights Reserved. 
#       Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#                
###############################################################################

# A simple A.B.C >= X.Y.Z version checker script.
# Copyright (c) 2006
# CSR plc.

usage()
{
    echo "a.b.c >= x.y.z version checker script."
    echo " "
    echo "  Usage: $0 <a.b.c> <x.y.z>"
    echo " "
    echo "  Returns 1 if <a.b.c> is greater than or equal to <x.y.z>"
    echo "  Otherwise, return 0. Result is stored in the \$? variable"
    echo " "
    exit 1
}

# Trivial check
if [ -z $1 -o -z $2 ]; then
    usage
fi

A=`echo $1|cut -d'.' -f1`
B=`echo $1|cut -d'.' -f2`
C=`echo $1|cut -d'.' -f3`
X=`echo $2|cut -d'.' -f1`
Y=`echo $2|cut -d'.' -f2`
Z=`echo $2|cut -d'.' -f3`

# Check each sub-version
if [ -z $A ]; then
    usage
fi
if [ -z $B ]; then
    usage
fi
if [ -z $C ]; then
    usage
fi
if [ -z $X ]; then
    usage
fi
if [ -z $Y ]; then
    usage
fi
if [ -z $Z ]; then
    usage
fi

# Major number, catch A<X and A>X
if [ $A -lt $X ]; then
    echo 0
    exit 0
fi
if [ $A -gt $X ]; then
    echo 1
    exit 0
fi

# Minor number, catch B<Y and B>Y
if [ $B -lt $Y ]; then
    echo 0
    exit 0
fi
if [ $B -gt $Y ]; then
    echo 1
    exit 0
fi

# Patch number, catch C>=Z
if [ $C -ge $Z ]; then
    echo 1
    exit 0
fi
echo 0
