// Name: Haifaa Abushaaban

#ifndef SVGHELPERA2_H
#define SVGHELPERA2_H

#include "SVGParser.h"
#include "SVGHelper.h"

// Module 1 helper functions:

// Function to validate xml file against a schema file
bool validateFileSVG(xmlDocPtr doc, const char* schemaFile);
xmlDocPtr createXMLFromStruct(const SVG* img);

// Functions to write svg shape struct to a node in the xml tree
void addAttrListToParentNode(List* list, xmlNodePtr* parent);
void addRectListToParentNode(List* rectList, xmlNodePtr* parent);
void addCircListToParentNode(List* circList, xmlNodePtr* parent);
void addPathListToParentNode(List* pathList, xmlNodePtr* parent);
void addGroupListToParentNode(List* groupList, xmlNodePtr* parent);
// Function to convert floating point number and units to a string
char* unitsWithNumber(float number, char units[]);

// Functions to check for validity for the structs and lists in SVGParser.h
bool validSVGStruct(const SVG* svg);
bool validAttrStruct(Attribute* attr);
bool validRectStruct(Rectangle* rect);
bool validCircStruct(Circle* circ);
bool validPathStruct(Path* path);
bool validGroupStruct(Group* group);

bool validAttrListStruct(List* otherAttributes);
bool validRectListStruct(List* rectangles);
bool validCircListStruct(List* circles);
bool validPathListStruct(List* paths);
bool validGroupListStruct(List* groupList);

bool isListEmpty(List* list);
bool checkString(char* string);
bool checkRange(float number);
bool checkInvalid(float number);

// Functions to help in the setting of attributes
bool changeValueInAttr (List* attrList, Attribute* newAttribute);
bool changeValueInRect (List* rectList, int index, Attribute* newAttribute);
bool changeValueInCirc (List* circList, int index, Attribute* newAttribute);
bool changeValueInPath (List* pathList, int index, Attribute* newAttribute);
bool changeValueInGroup (List* groupList, int index, Attribute* newAttribute);

#endif
