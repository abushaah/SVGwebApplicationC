// Name: Haifaa Abushaaban [1146372]

#ifndef SVGHELPER2_H
#define SVGHELPER2_H

#include "SVGParser.h"

// Module 1 helper functions:

// Functions to write svg shape struct to a node in the xml tree
void addAttrListToParentNode(List* list, xmlNodePtr* parent);
void addRectListToParentNode(List* rectList, xmlNodePtr* parent);
void addCircListToParentNode(List* circList, xmlNodePtr* parent);
void addPathListToParentNode(List* pathList, xmlNodePtr* parent);
void addGroupListToParentNode(List* groupList, xmlNodePtr* parent);
// Function to convert floating point number and units to a string
char* unitsWithNumber(float number, char units[]);

#endif
