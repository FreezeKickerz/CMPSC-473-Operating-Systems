//Name: Tyler Korz
//PSU Email: tkk5297@psu.edu    
//PSU ID: 967407712

#include "kobject.h"

/* Initialize kobject */
void kobject_init(struct kobject *kobj, const char *name, struct kobject *parent) {
    strncpy(kobj->name, name, sizeof(kobj->name));
    kobj->parent = parent;
    kobj->refcount = 1; // Initial reference count
    kobj->release = NULL;
    if (parent) {
        parent->refcount++;
    }
}

/* Initialize kset */
void kset_init(struct kset *kset, const char *name, struct kobject *parent) {
    strncpy(kset->name, name, sizeof(kset->name));
    kobject_init(&kset->kobj, name, parent);
    kset->members = NULL;
    kset->member_count = 0;
}

/* Add kobject to kset */
void kset_add(struct kset *kset, struct kobject *kobj) {
    kset->members = realloc(kset->members, sizeof(struct kobject *) * (kset->member_count + 1));
    kset->members[kset->member_count] = kobj;
    kset->member_count++;
    printf("Added kobject '%s' to kset '%s'.\n", kobj->name, kset->name);
}

/* Initialize Device */
void device_init(struct Device *device, const char *name, struct kobject *parent) {
    //TODO: IMPLEMENT THIS
    kobject_init(&device->kobj, name, parent);
    printf("Initialized Device '%s'.\n", name);
}

/* Initialize Bus */
void bus_init(struct Bus *bus, const char *name) {
    //TODO: IMPLEMENT THIS
    kset_init(&bus->kset, name, NULL);
    bus->bridge = NULL;
    printf("Initialized Bus '%s'.\n", name);
}

/* Add Device to Bus */
void bus_add_device(struct Bus *bus, struct Device *device) {
    //TODO: IMPLEMENT THIS
    kset_add(&bus->kset, &device->kobj);
}

/* Print hierarchy recursively */
void print_hierarchy(const struct kobject *kobj, int level) {
    //TODO: IMPLEMENT
    for (int i = 0; i < level; i++)
        printf("  ");
    printf("kobject: %s\n", kobj->name);
}

/* Print Bus hierarchy */
void bus_print_hierarchy(const struct Bus *bus) {
    //TODO: IMPLEMENT THIS
    printf("Hierarchy of Bus '%s':\n", bus->kset.name);
//Print the bus container
 printf("Hierarchy of Bus '%s':\n", bus->kset.name);
    print_hierarchy(&bus->kset.kobj, 0);
    
    if (bus->bridge != NULL) {
         print_hierarchy(&bus->bridge->kobj, 1);
         if (bus->bridge->kobj.parent)
             print_hierarchy(bus->bridge->kobj.parent, 2);
    }
    
    for (size_t i = 0; i < bus->kset.member_count; i++) {
         struct kobject *child = bus->kset.members[i];
         print_hierarchy(child, 1);
         if (child->parent)
             print_hierarchy(child->parent, 2);
         if (bus->bridge != NULL && child->parent == &bus->kset.kobj) {
             print_hierarchy(&bus->bridge->kobj, 3);
             if (bus->bridge->kobj.parent)
                 print_hierarchy(bus->bridge->kobj.parent, 4);
         }
    }
}

void bridge_init(struct Bridge *bridge, const char *name, struct Bus *parent_bus) {
    //TODO: IMPLEMENT THIS
    kobject_init(&bridge->kobj, name, &parent_bus->kset.kobj);
    printf("Initialized Device '%s'.\n", name);
}

