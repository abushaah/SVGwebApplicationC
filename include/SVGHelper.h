// Name: Hafaa Abushaaban [1146372]

#ifndef SVGHELPER_H
#define SVGHELPER_H

#include "SVGParser.h"

// Module 1 helper functions:

// svg parser function that loops through and creates the structs
int get_element_names(xmlNode* a_node, SVG* svg, Group** group, int* groupIdx, int* inserted);

// struct creation functions
Attribute* otherAttributes (char *name, char *content);
Rectangle* rectAttributes(xmlNode *cur_node);
Circle* circAttributes(xmlNode *cur_node);
Path* pathAttributes (xmlNode *cur_node);
Group* groupAttributes (xmlNode *cur_node);

// list within a struct creation functions
void firstOtherAttributes(xmlNode *cur_node, List* otherAttributesList);

// validity check functions
int numberWithUnits(float* number, char* units, char* value);
int titleDescNS(char *field, char* data);

int validChar(char* word);
int emptyString(char* word);

// Module 2 helper funcitons:
int getElementGroups(List *source, List *dest, char* type);
int findNumShape(List * list, bool (*customCompare)(const void* first,const void* second), const void* searchRecord);

bool compareRectAreas(const void* data, const void* area);
bool compareCircAreas(const void* data, const void* area);
bool comparePathData(const void* data, const void* string);
bool compareGroupLen(const void* data, const void* length);
bool compareAttr(const void* data, const void* length);
int getRectAttrLen(List* list);
int getCircAttrLen(List* list);
int getPathAttrLen(List* list);
int getGroupAttrLen(List* list);

int compareInGroups(List *group, bool (*customCompare)(const void* first, const void* second), const void* searchRecord, char* type);

#endif
