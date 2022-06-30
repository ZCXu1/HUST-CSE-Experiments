typedef unsigned           int uint32_t;
typedef void (* TaskFunction_t)( void * );
void prvSetupHardware( void );
void StartFreeRTOS( uint32_t id,TaskFunction_t vTask3 );
