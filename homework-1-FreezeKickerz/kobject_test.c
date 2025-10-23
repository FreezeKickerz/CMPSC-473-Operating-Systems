#include "kobject.h"



/* Main program */
int main() {
    /* Create a bus */
    struct Bus bus;
    bus_init(&bus, "BusA");

     /* Create devices */
    struct Device device1, device2;
    
    //TODO: Initialize devices with appropriate name and parent

    /* Create a bus */
    struct Bus bus2;
    bus_init(&bus2, "BusB");

    struct Bridge bridge1;
    bridge_init(&bridge1, "BusA2BusB", &bus);

   
    // TODO: How do the bridge and buses fit into the hierarchy

    struct Device device3;

    // TODO: Initialize device with appropriate name and parent


    // TODO: Set up appropriate hierarchy for all
     

    /* Print the hierarchy */
    bus_print_hierarchy(&bus);
    bus_print_hierarchy(&bus2);

    printf("Please make sure that the print functions and device, bus or bridge names are not hardcoded.\n");
    return 0;
}