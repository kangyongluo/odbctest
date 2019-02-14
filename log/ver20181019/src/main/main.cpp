#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../case/testODBC.h"
#include "../util/version.h"

void version(void)
{
    char version[60] = {0};

    snprintf(version, sizeof(version), "%s build:%c%c%c%c-%c%c-%c%c %c%c:%c%c:%c%c",
                                        CONFIG_VERSION_STRING,
                                        BUILD_YEAR_CH0,
                                        BUILD_YEAR_CH1,
                                        BUILD_YEAR_CH2,
                                        BUILD_YEAR_CH3,

                                        BUILD_MONTH_CH0,
                                        BUILD_MONTH_CH1,

                                        BUILD_DAY_CH0,
                                        BUILD_DAY_CH1,

                                        BUILD_HOUR_CH0,
                                        BUILD_HOUR_CH1,

                                        BUILD_MIN_CH0,
                                        BUILD_MIN_CH1,

                                        BUILD_SEC_CH0,
                                        BUILD_SEC_CH1
                                        );
    printf("%s\n", version);
}

void help(void)
{
    version();
    printf("\t-h:print this help\n");
    printf("\t-v:print odbcPerformance version\n");
    printf("Connection related options. You can connect using either:\n");
    printf("\t-d Data_Source_Name\n");
    printf("\t-u user\n");
    printf("\t-p password\n");
    printf("\t-s schema\n");
}
int main (int argc, char *argv[])
{
    CTestODBC *testODBC = new CTestODBC;
    
    printf("Testing odbc, please don't shut down.\n");
    testODBC->runTest();

    delete testODBC;
    
    return 0;
}
