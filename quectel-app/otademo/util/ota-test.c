#include <stdio.h>
#include <ql_ota.h>

int main (int argc, char **argv)
{
    QL_OTA_Set_Package_Path("/data/update_ext4.zip");

    QL_OTA_Start_Update();

    return 0;
} /* ----- End of main() ----- */

