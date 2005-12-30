
"""
 Copyright (C) 2005 Eric Ehlers
 Copyright (C) 2005 Plamen Neykov
 Copyright (C) 2005 Aurelien Chanudet

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email quantlib-dev@lists.sf.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
"""

"""A class which implements the (De)Serializer interface to deserialize
a (De)Serializable object from an XML stream."""

import serializer
import factory
import xml.dom.minidom
import sys
import common

class XmlReader(serializer.Serializer):
    """A class which implements the (De)Serializer interface to deserialize
    a (De)Serializable object from an XML stream."""

    def __init__(self, fileName):
        """load indicated file into dom document."""
        self.documentName = fileName + '.xml'
        try:
            dom = xml.dom.minidom.parse(self.documentName)
        except:
            print 'error processing document ' + self.documentName
            raise
        self.node = dom.documentElement

    def serializeAttribute(self, dict, attributeName, defaultValue = None):
        """read a named attribute."""
        attributeValue = self.node.getAttribute(attributeName)
        if attributeValue:
            dict[attributeName] = attributeValue
        else:
            dict[attributeName] = defaultValue

    def serializeAttributeInteger(self, dict, attributeName, defaultValue = None):
        """read a named integral attribute."""
        attributeValue = self.node.getAttribute(attributeName)
        if attributeValue:
            dict[attributeName] = int(attributeValue)
        else:
            dict[attributeName] = defaultValue

    def serializeAttributeBoolean(self, dict, attributeName):
        """read a named boolean attribute."""
        attributeValue = self.node.getAttribute(attributeName)
        if attributeValue:
            dict[attributeName] = self.stringToBoolean(attributeValue)
        else:
            dict[attributeName] = False

    def serializeProperty(self, dict, propertyName, defaultValue = None):
        """read a named property."""
        element = self.getChild(propertyName, True)
        if element:
            dict[propertyName] = self.getNodeValue(element)
        else:
            dict[propertyName] = defaultValue

    def serializeBoolean(self, dict, propertyName):
        """read a named boolean property."""
        element = self.getChild(propertyName, True)
        if element:
            dict[propertyName] = self.stringToBoolean(self.getNodeValue(element))
        else:
            dict[propertyName] = False

    def serializeList(self, dict, vectorName, itemName):
        """read a list of elements."""
        vectorElement = self.getChild(vectorName)
        itemElements = vectorElement.getElementsByTagName(itemName)
        dict[vectorName] = []
        for itemElement in itemElements:
            dict[vectorName].append(self.getNodeValue(itemElement))

    def serializeDict(self, dict, dictName):
        """read a named element in the document and write its values as
        key/value pairs in the given dict"""
        dictElement = self.getChild(dictName)
        dict[dictName] = {}
        for childNode in dictElement.childNodes:
            if self.isTextNode(childNode):
                dict[dictName][childNode.nodeName] = self.getNodeValue(childNode)

    def serializeObject(self, dict, objectClass):
        """load a Serializable object."""
        objectElement = self.getChild(objectClass.__name__)
        objectInstance = objectClass()
        self.node = objectElement
        objectInstance.serialize(self)
        objectInstance.postSerialize()
        self.node = self.node.parentNode
        dict[objectInstance.key()] = objectInstance

    def serializeObjectList(self, dict, objectClass):
        """load a list of Serializable objects."""
        listElement = self.getChild(objectClass.groupName)
        itemElements = listElement.getElementsByTagName(objectClass.__name__)
        dict[objectClass.groupName] = []
        for itemElement in itemElements:
            objectInstance = objectClass()
            self.node = itemElement
            objectInstance.serialize(self)
            objectInstance.postSerialize()
            self.node = self.node.parentNode.parentNode
            dict[objectClass.groupName].append(objectInstance)
        dict[objectClass.__name__ + 'Count'] = len(dict[objectClass.groupName])

    def serializeObjectDict(self, dict, objectClass):
        """load a named element and write its data to the given dictionary.  

        A client invokes this function as follows:
            serializer.serializeObjectDict(self.__dict__, Foo)

        and after the call returns, the caller's dict is populated as follows:
            self.Foo - a dict of objects, keyed by object identifier
            self.FooKeys - a sorted list of the object identifiers
            self.FooCount - #/objects loaded

        these structures allow the caller to iterate through the objects
        in order by identifier."""

        dictElement = self.getChild(objectClass.groupName)
        dict[objectClass.groupName] = {}
        dict[objectClass.__name__ + 'Keys'] = []
        for childNode in dictElement.childNodes:
            if childNode.nodeName == '#text': continue
            objectInstance = factory.Factory.getInstance().makeObject(childNode.nodeName)
            self.node = childNode
            objectInstance.serialize(self)
            objectInstance.postSerialize()
            self.node = self.node.parentNode.parentNode
            dict[objectClass.groupName][objectInstance.key()] = objectInstance
            dict[objectClass.__name__ + 'Keys'].append(objectInstance.key())
        dict[objectClass.__name__ + 'Keys'].sort()
        dict[objectClass.__name__ + 'Count'] = len(dict[objectClass.__name__ + 'Keys'])

    def serializeObjectPropertyDict(self, dict, objectClass):
        """ load a named element and write its children directly as
        properties of the calling object.

        A client invokes this function as follows:
            serializer.serializeObjectPropertyDict(self.__dict__, Foo)

        For xml data formatted as follows:
            <FooList>
                <Foo name='fooIdentifier_1'/>
                <Foo name='fooIdentifier_2'/>
                ...
                <Foo name='fooIdentifier_n'/>
            </FooList>

        And after the call returns, the caller's dict is populated as follows:
            self.fooIdentifier_1
            self.fooIdentifier_2
            ...
            self.fooIdentifier_n

        This is appropriate when the caller knows at compile time the names
        of the objects to be loaded."""

        dictElement = self.getChild(objectClass.groupName)
        itemElements = dictElement.getElementsByTagName(objectClass.__name__)
        for itemElement in itemElements:
            objectInstance = objectClass()
            self.node = itemElement
            objectInstance.serialize(self)
            objectInstance.postSerialize()
            self.node = self.node.parentNode.parentNode
            dict[objectInstance.key()] = objectInstance

    def getChild(self, tagName, allowNone = False):
        """get single named child of current node."""
        for childNode in self.node.childNodes:
            if childNode.nodeName == tagName:
                return childNode
        if not allowNone:
            self.abort('no element with name "%s"' % tagName)

    def getNodeValue(self, node):
        """get value of text node."""
        if node.nodeType == xml.dom.Node.ELEMENT_NODE \
        and len(node.childNodes) == 1 \
        and node.firstChild.nodeType == xml.dom.Node.TEXT_NODE:
            return node.firstChild.nodeValue
        else:
            self.abort('error processing node "%s"' % node.nodeName)

    def stringToBoolean(self, str):
        """convert text string to boolean."""
        if str.lower() == common.TRUE:
            return True
        elif str.lower() == common.FALSE:
            return False
        else:
            self.abort('unable to convert string "%s" to boolean' % str)

    def isTextNode(self, node):
        """return True for element node with single child text node."""
        return node.nodeType == xml.dom.Node.ELEMENT_NODE \
        and len(node.childNodes) == 1 \
        and node.firstChild.nodeType == xml.dom.Node.TEXT_NODE

    def abort(self, errorMessage):
        """identify the xml document before failing."""
        sys.exit('error loading XML document %s : %s' 
            % (self.documentName, errorMessage))

