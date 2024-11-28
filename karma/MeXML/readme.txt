Project requirements
--------------------

- don't have to create toolkit objects, ie user can do what they want with loaded data
- An API for adding and removing search paths
- no hierarchy info in the file format
- No explicit references between files
- an archiving mechanism which allows loading from a directory or from a zip
- all toolkit objects must be contained in the file format
- groups of models and constraints must be supported
- the format should be in XML
- read from memory or file
- resultant files are designed to be embeddable rather than be embedded into
- resultant code should be packaged for reuse
- there should be no order dependency in files or in the saving API


Documentation
-------------

1. MeFile2
----------

The purpose of MeFile2 is to manage a set of data structures which mirror the Karma toolkit
data structures. Instances of these data structures can be:

- created from toolkit objects, 
- created from a memory block of formatted XML
- destroyed
- written to a memory block as formatted XML
- looked up by name
- inserted and removed from MeFile2 management
- used to create toolkit objects

MeFile2 provides a service which resolves all references after loading but before
toolkit objects are created. It also provides a service to resolve all references
prior to saving. 


Implementation details
----------------------

A MeFile object contains 3 maps from string to pointer, one for geometry, one for models
and one for constraints. The maps are used for quick lookup by name. Because there are three
maps, IDs do not have to be globally unique. In addition to these maps there will also be
three doubly linked lists. These lists are used for ordered iteration to ensure that if a
MeFile object is loaded and then saved, the resulting file will be identical.

MeFile2 depends on the core toolkit libraries.

There is no order dependency in the file. This means that loading is done in two stages. First
the file is parsed to typed data, and then any data fields that could not be set in the first 
pass will be set in the second pass when all the data will be available. Aggregate geometry
will use the second pass in order to ensure that normal geometry and aggregate geometry can
be specified in any order.

Addendum: There is no load time order dependency, but when the file is saved geometries
are outputted first, then models then constraints. The ordering within these categories
is preserved.

There are functions provided for retrieving a specified geometry, model or joint from a
given file.

Dangling references are allowed in files. It is up to the user to resolve them by
searching other MeFiles for the referent.

Inserting a model will insert its geometry if not already inserted.


MeFile2 File format features
---------------------------

All toolkit objects are supported.

IDs are required for all items, in the form of an attribute.

Constraints can be specified in local or world coordinates.

The file format is designed to be embedded rather than embeddable. To this end there is no
provision for user data in the file. The only item present that is not a toolkit object is
a graphic for each model.

Groups of models and joints with relative transforms is supported.

Example
-------

<KARMA>
    <GEOMETRY>
        <AGGREGATE id="ag1">
            <ELEMENT geometry="box01">
                <TM>1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1</TM>
            </ELEMENT>
            <ELEMENT geometry="box02">
                <TM>1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1</TM>
            </ELEMENT>
        </AGGREGATE>
        <BOX id="box01">
            <DIMS>1,1,1</DIMS>
        <BOX>
        <BOX id="box02">
            <DIMS>2,2,2</DIMS>
        <BOX>
    </GEOMETRY>
    <MODELS>
        <MODEL id="dynamics_and_geometry" geometry="box" graphic="some_graphic">
            <TM>1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1</TM>
            <GEOM_TM>1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1</GEOM_TM>
            <DYNAMICS>
                <MASS_TM>1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1</MASS_TM>
                <MASS>1</MASS>
                <INERTIA>0.4,0,0,0.4,0,0.4</INERTIA>
		        <LIN_DAMP>0</LIN_DAMP>
		        <ANG_DAMP>0</ANG_DAMP>
		        <FAST_SPIN>0,0,0</FAST_SPIN>
		        <FORCE>0,0,0</FORCE>
		        <TORQUE>0,0,0</TORQUE>
		        <LIN_VEL>0,0,0</LIN_VEL>
		        <ANG_VEL>0,0,0</ANG_VEL>
            </DYNAMICS>
        </MODEL>
        <MODEL id="just_geometry" geometry="box" graphic="something">
            <TM>1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1</TM>
            <GEOM_TM>1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1</GEOM_TM>
        </MODEL>
        <MODEL id="just_dynamics" graphic="some_graphic>
            <TM>1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1</TM>
            <DYNAMICS>
                <MASS_TM>1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1</MASS_TM>
                <MASS>1</MASS>
                <INERTIA>0.4,0,0,0.4,0,0.4</INERTIA>
			    <LIN_DAMP>0</LIN_DAMP>
			    <ANG_DAMP>0</ANG_DAMP>
			    <FAST_SPIN>0,0,0</FAST_SPIN>
			    <FORCE>0,0,0</FORCE>
			    <TORQUE>0,0,0</TORQUE>
			    <LIN_VEL>0,0,0</LIN_VEL>
			    <ANG_VEL>0,0,0</ANG_VEL>
            </DYNAMICS>
        </MODEL>
    <MODELS>
    <CONSTRAINTS>
        <HINGE id="hinge1" body="crane_base" body="world">
            <POS>1,0,0</POS>  
            <AXIS>1,0,0</AXIS>  
        </HINGE>
    </CONSTRAINTS>
    <GROUP>
        <ELEMENT id="crane_base">
            <TM>1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1</TM>
        </ELEMENT>
        <ELEMENT id="hinge1">
            <TM>1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1</TM>
        </ELEMENT>
     </GROUP>
</KARMA>


2. MeXML
--------

The purpose of MeXML is twofold. Firstly it is responsible for parsing XML from a memory block into
typed structures provided by another library. In this case this other library is MeFile2. Secondly
it is provides the functionality for writing formatted XML (including attributes) to a memory block.

The XML parser has some limitations. It can only support attributes for an element that contains
other elements. It can only parse a subset of XML. There is no type of handler that can perform
the same operation on either all elements or on all child elements of a certain element.


3. MeArchive
------------ 



FAQ
---

Can you read as many files as you like into a MeFile object? What happens? Do I ever need to create
more than one?

What happens when I destroy a MeFile object? Do any transforms get destroyed? When should
I destroy a MeFile object?

Are there going to be times when the user will want to create an FGeometry not from
a toolkit geometry? Yes, definitely. There should be an API to create a sphere
eg. FGeometry *CreateFSphere(char *id, MeReal radius)

Testplan
--------

Test:

- nested aggregates
- memory cleanup
- appropriate warning messages are generated

TODO:
Toolkit defaults header with #defines