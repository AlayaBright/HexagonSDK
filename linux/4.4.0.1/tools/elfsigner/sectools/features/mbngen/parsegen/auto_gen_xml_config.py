#!/usr/bin/env python
# -*- coding: utf-8 -*- 

#
# Generated  by generateDS.py version 2.7b.
#
from past.builtins import basestring

import sys
import re as re_

from six import ensure_str

etree_ = None
Verbose_import_ = False
(   XMLParser_import_none, XMLParser_import_lxml,
    XMLParser_import_elementtree
    ) = range(3)
XMLParser_import_library = None
try:
    # lxml
    from lxml import etree as etree_
    XMLParser_import_library = XMLParser_import_lxml
    if Verbose_import_:
        print("running with lxml.etree")
except ImportError:
    try:
        # cElementTree from Python 2.5+
        import xml.etree.cElementTree as etree_
        XMLParser_import_library = XMLParser_import_elementtree
        if Verbose_import_:
            print("running with cElementTree on Python 2.5+")
    except ImportError:
        try:
            # ElementTree from Python 2.5+
            import xml.etree.ElementTree as etree_
            XMLParser_import_library = XMLParser_import_elementtree
            if Verbose_import_:
                print("running with ElementTree on Python 2.5+")
        except ImportError:
            try:
                # normal cElementTree install
                import cElementTree as etree_
                XMLParser_import_library = XMLParser_import_elementtree
                if Verbose_import_:
                    print("running with cElementTree")
            except ImportError:
                try:
                    # normal ElementTree install
                    import elementtree.ElementTree as etree_
                    XMLParser_import_library = XMLParser_import_elementtree
                    if Verbose_import_:
                        print("running with ElementTree")
                except ImportError:
                    raise ImportError("Failed to import ElementTree from any known place")

def parsexml_(*args, **kwargs):
    if (XMLParser_import_library == XMLParser_import_lxml and
        'parser' not in kwargs):
        # Use the lxml ElementTree compatible parser so that, e.g.,
        #   we ignore comments.
        kwargs['parser'] = etree_.ETCompatXMLParser()
    doc = etree_.parse(*args, **kwargs)
    return doc

#
# User methods
#
# Calls to the methods in these classes are generated by generateDS.py.
# You can replace these methods by re-implementing the following class
#   in a module named generatedssuper.py.

try:
    from generatedssuper import GeneratedsSuper
except ImportError as exp:

    class GeneratedsSuper(object):
        def gds_format_string(self, input_data, input_name=''):
            return ensure_str(input_data)
        def gds_validate_string(self, input_data, node, input_name=''):
            return input_data
        def gds_format_integer(self, input_data, input_name=''):
            return '%d' % input_data
        def gds_validate_integer(self, input_data, node, input_name=''):
            return input_data
        def gds_format_integer_list(self, input_data, input_name=''):
            return '%s' % input_data
        def gds_validate_integer_list(self, input_data, node, input_name=''):
            values = input_data.split()
            for value in values:
                try:
                    fvalue = float(value)
                except (TypeError, ValueError) as exp:
                    raise_parse_error(node, 'Requires sequence of integers')
            return input_data
        def gds_format_float(self, input_data, input_name=''):
            return '%f' % input_data
        def gds_validate_float(self, input_data, node, input_name=''):
            return input_data
        def gds_format_float_list(self, input_data, input_name=''):
            return '%s' % input_data
        def gds_validate_float_list(self, input_data, node, input_name=''):
            values = input_data.split()
            for value in values:
                try:
                    fvalue = float(value)
                except (TypeError, ValueError) as exp:
                    raise_parse_error(node, 'Requires sequence of floats')
            return input_data
        def gds_format_double(self, input_data, input_name=''):
            return '%e' % input_data
        def gds_validate_double(self, input_data, node, input_name=''):
            return input_data
        def gds_format_double_list(self, input_data, input_name=''):
            return '%s' % input_data
        def gds_validate_double_list(self, input_data, node, input_name=''):
            values = input_data.split()
            for value in values:
                try:
                    fvalue = float(value)
                except (TypeError, ValueError) as exp:
                    raise_parse_error(node, 'Requires sequence of doubles')
            return input_data
        def gds_format_boolean(self, input_data, input_name=''):
            return '%s' % input_data
        def gds_validate_boolean(self, input_data, node, input_name=''):
            return input_data
        def gds_format_boolean_list(self, input_data, input_name=''):
            return '%s' % input_data
        def gds_validate_boolean_list(self, input_data, node, input_name=''):
            values = input_data.split()
            for value in values:
                if value not in ('true', '1', 'false', '0', ):
                    raise_parse_error(node, 'Requires sequence of booleans ("true", "1", "false", "0")')
            return input_data
        def gds_str_lower(self, instring):
            return instring.lower()
        def get_path_(self, node):
            path_list = []
            self.get_path_list_(node, path_list)
            path_list.reverse()
            path = '/'.join(path_list)
            return path
        Tag_strip_pattern_ = re_.compile(r'\{.*\}')
        def get_path_list_(self, node, path_list):
            if node is None:
                return
            tag = GeneratedsSuper.Tag_strip_pattern_.sub('', node.tag)
            if tag:
                path_list.append(tag)
            self.get_path_list_(node.getparent(), path_list)
        def get_class_obj_(self, node, default_class=None):
            class_obj1 = default_class
            if 'xsi' in node.nsmap:
                classname = node.get('{%s}type' % node.nsmap['xsi'])
                if classname is not None:
                    names = classname.split(':')
                    if len(names) == 2:
                        classname = names[1]
                    class_obj2 = globals().get(classname)
                    if class_obj2 is not None:
                        class_obj1 = class_obj2
            return class_obj1
        def gds_build_any(self, node, type_name=None):
            return None


#
# If you have installed IPython you can uncomment and use the following.
# IPython is available from http://ipython.scipy.org/.
#

## from IPython.Shell import IPShellEmbed
## args = ''
## ipshell = IPShellEmbed(args,
##     banner = 'Dropping into IPython',
##     exit_msg = 'Leaving Interpreter, back to program.')

# Then use the following line where and when you want to drop into the
# IPython shell:
#    ipshell('<some message> -- Entering ipshell.\nHit Ctrl-D to exit')

#
# Globals
#

ExternalEncoding = 'ascii'
Tag_pattern_ = re_.compile(r'({.*})?(.*)')
String_cleanup_pat_ = re_.compile(r"[\n\r\s]+")
Namespace_extract_pat_ = re_.compile(r'{(.*)}(.*)')

#
# Support/utility functions.
#

def showIndent(outfile, level):
    for idx in range(level):
        outfile.write('    ')

def quote_xml(inStr):
    if not inStr:
        return ''
    s1 = (isinstance(inStr, basestring) and inStr or
          '%s' % inStr)
    s1 = ensure_str(s1)
    s1 = s1.replace('&', '&amp;')
    s1 = s1.replace('<', '&lt;')
    s1 = s1.replace('>', '&gt;')
    return s1

def quote_attrib(inStr):
    s1 = (isinstance(inStr, basestring) and inStr or
          '%s' % inStr)
    s1 = ensure_str(s1)
    s1 = s1.replace('&', '&amp;')
    s1 = s1.replace('<', '&lt;')
    s1 = s1.replace('>', '&gt;')
    if '"' in s1:
        if "'" in s1:
            s1 = '"%s"' % s1.replace('"', "&quot;")
        else:
            s1 = "'%s'" % s1
    else:
        s1 = '"%s"' % s1
    return s1

def quote_python(inStr):
    s1 = inStr
    if s1.find("'") == -1:
        if s1.find('\n') == -1:
            return "'%s'" % s1
        else:
            return "'''%s'''" % s1
    else:
        if s1.find('"') != -1:
            s1 = s1.replace('"', '\\"')
        if s1.find('\n') == -1:
            return '"%s"' % s1
        else:
            return '"""%s"""' % s1

def get_all_text_(node):
    if node.text is not None:
        text = node.text
    else:
        text = ''
    for child in node:
        if child.tail is not None:
            text += child.tail
    return text

def find_attr_value_(attr_name, node):
    attrs = node.attrib
    attr_parts = attr_name.split(':')
    value = None
    if len(attr_parts) == 1:
        value = attrs.get(attr_name)
    elif len(attr_parts) == 2:
        prefix, name = attr_parts
        namespace = node.nsmap.get(prefix)
        if namespace is not None:
            value = attrs.get('{%s}%s' % (namespace, name, ))
    return value


class GDSParseError(Exception):
    pass

def raise_parse_error(node, msg):
    if XMLParser_import_library == XMLParser_import_lxml:
        msg = '%s (element %s/line %d)' % (msg, node.tag, node.sourceline, )
    else:
        msg = '%s (element %s)' % (msg, node.tag, )
    raise GDSParseError(msg)


class MixedContainer:
    # Constants for category:
    CategoryNone = 0
    CategoryText = 1
    CategorySimple = 2
    CategoryComplex = 3
    # Constants for content_type:
    TypeNone = 0
    TypeText = 1
    TypeString = 2
    TypeInteger = 3
    TypeFloat = 4
    TypeDecimal = 5
    TypeDouble = 6
    TypeBoolean = 7
    def __init__(self, category, content_type, name, value):
        self.category = category
        self.content_type = content_type
        self.name = name
        self.value = value
    def getCategory(self):
        return self.category
    def getContenttype(self, content_type):
        return self.content_type
    def getValue(self):
        return self.value
    def getName(self):
        return self.name
    def export(self, outfile, level, name, namespace):
        if self.category == MixedContainer.CategoryText:
            # Prevent exporting empty content as empty lines.
            if self.value.strip(): 
                outfile.write(self.value)
        elif self.category == MixedContainer.CategorySimple:
            self.exportSimple(outfile, level, name)
        else:    # category == MixedContainer.CategoryComplex
            self.value.export(outfile, level, namespace,name)
    def exportSimple(self, outfile, level, name):
        if self.content_type == MixedContainer.TypeString:
            outfile.write('<%s>%s</%s>' % (self.name, self.value, self.name))
        elif self.content_type == MixedContainer.TypeInteger or \
                self.content_type == MixedContainer.TypeBoolean:
            outfile.write('<%s>%d</%s>' % (self.name, self.value, self.name))
        elif self.content_type == MixedContainer.TypeFloat or \
                self.content_type == MixedContainer.TypeDecimal:
            outfile.write('<%s>%f</%s>' % (self.name, self.value, self.name))
        elif self.content_type == MixedContainer.TypeDouble:
            outfile.write('<%s>%g</%s>' % (self.name, self.value, self.name))
    def exportLiteral(self, outfile, level, name):
        if self.category == MixedContainer.CategoryText:
            showIndent(outfile, level)
            outfile.write('model_.MixedContainer(%d, %d, "%s", "%s"),\n' % \
                (self.category, self.content_type, self.name, self.value))
        elif self.category == MixedContainer.CategorySimple:
            showIndent(outfile, level)
            outfile.write('model_.MixedContainer(%d, %d, "%s", "%s"),\n' % \
                (self.category, self.content_type, self.name, self.value))
        else:    # category == MixedContainer.CategoryComplex
            showIndent(outfile, level)
            outfile.write('model_.MixedContainer(%d, %d, "%s",\n' % \
                (self.category, self.content_type, self.name,))
            self.value.exportLiteral(outfile, level + 1)
            showIndent(outfile, level)
            outfile.write(')\n')


class MemberSpec_(object):
    def __init__(self, name='', data_type='', container=0):
        self.name = name
        self.data_type = data_type
        self.container = container
    def set_name(self, name): self.name = name
    def get_name(self): return self.name
    def set_data_type(self, data_type): self.data_type = data_type
    def get_data_type_chain(self): return self.data_type
    def get_data_type(self):
        if isinstance(self.data_type, list):
            if len(self.data_type) > 0:
                return self.data_type[-1]
            else:
                return 'xs:string'
        else:
            return self.data_type
    def set_container(self, container): self.container = container
    def get_container(self): return self.container

def _cast(typ, value):
    if typ is None or value is None:
        return value
    return typ(value)

#
# Data representation classes.
#

class mbngen(GeneratedsSuper):
    subclass = None
    superclass = None
    def __init__(self, mbngen_conf=None):
        self.mbngen_conf = mbngen_conf
    def factory(*args_, **kwargs_):
        if mbngen.subclass:
            return mbngen.subclass(*args_, **kwargs_)
        else:
            return mbngen(*args_, **kwargs_)
    factory = staticmethod(factory)
    def get_mbngen_conf(self): return self.mbngen_conf
    def set_mbngen_conf(self, mbngen_conf): self.mbngen_conf = mbngen_conf
    def export(self, outfile, level, namespace_='tns:', name_='mbngen', namespacedef_=''):
        showIndent(outfile, level)
        outfile.write('<%s%s%s' % (namespace_, name_, namespacedef_ and ' ' + namespacedef_ or '', ))
        already_processed = []
        self.exportAttributes(outfile, level, already_processed, namespace_, name_='mbngen')
        if self.hasContent_():
            outfile.write('>\n')
            self.exportChildren(outfile, level + 1, namespace_, name_)
            showIndent(outfile, level)
            outfile.write('</%s%s>\n' % (namespace_, name_))
        else:
            outfile.write('/>\n')
    def exportAttributes(self, outfile, level, already_processed, namespace_='tns:', name_='mbngen'):
        pass
    def exportChildren(self, outfile, level, namespace_='tns:', name_='mbngen', fromsubclass_=False):
        if self.mbngen_conf is not None:
            self.mbngen_conf.export(outfile, level, namespace_, name_='mbngen_conf', )
    def hasContent_(self):
        if (
            self.mbngen_conf is not None
            ):
            return True
        else:
            return False
    def exportLiteral(self, outfile, level, name_='mbngen'):
        level += 1
        self.exportLiteralAttributes(outfile, level, [], name_)
        if self.hasContent_():
            self.exportLiteralChildren(outfile, level, name_)
    def exportLiteralAttributes(self, outfile, level, already_processed, name_):
        pass
    def exportLiteralChildren(self, outfile, level, name_):
        if self.mbngen_conf is not None:
            showIndent(outfile, level)
            outfile.write('mbngen_conf=model_.complex_mbngen_conf(\n')
            self.mbngen_conf.exportLiteral(outfile, level, name_='mbngen_conf')
            showIndent(outfile, level)
            outfile.write('),\n')
    def build(self, node):
        self.buildAttributes(node, node.attrib, [])
        for child in node:
            nodeName_ = Tag_pattern_.match(child.tag).groups()[-1]
            self.buildChildren(child, node, nodeName_)
    def buildAttributes(self, node, attrs, already_processed):
        pass
    def buildChildren(self, child_, node, nodeName_, fromsubclass_=False):
        if nodeName_ == 'mbngen_conf':
            obj_ = complex_mbngen_conf.factory()
            obj_.build(child_)
            self.set_mbngen_conf(obj_)
# end class mbngen


class complex_mbngen_conf(GeneratedsSuper):
    subclass = None
    superclass = None
    def __init__(self, conf=None):
        if conf is None:
            self.conf = []
        else:
            self.conf = conf
    def factory(*args_, **kwargs_):
        if complex_mbngen_conf.subclass:
            return complex_mbngen_conf.subclass(*args_, **kwargs_)
        else:
            return complex_mbngen_conf(*args_, **kwargs_)
    factory = staticmethod(factory)
    def get_conf(self): return self.conf
    def set_conf(self, conf): self.conf = conf
    def add_conf(self, value): self.conf.append(value)
    def insert_conf(self, index, value): self.conf[index] = value
    def export(self, outfile, level, namespace_='tns:', name_='complex_mbngen_conf', namespacedef_=''):
        showIndent(outfile, level)
        outfile.write('<%s%s%s' % (namespace_, name_, namespacedef_ and ' ' + namespacedef_ or '', ))
        already_processed = []
        self.exportAttributes(outfile, level, already_processed, namespace_, name_='complex_mbngen_conf')
        if self.hasContent_():
            outfile.write('>\n')
            self.exportChildren(outfile, level + 1, namespace_, name_)
            showIndent(outfile, level)
            outfile.write('</%s%s>\n' % (namespace_, name_))
        else:
            outfile.write('/>\n')
    def exportAttributes(self, outfile, level, already_processed, namespace_='tns:', name_='complex_mbngen_conf'):
        pass
    def exportChildren(self, outfile, level, namespace_='tns:', name_='complex_mbngen_conf', fromsubclass_=False):
        for conf_ in self.conf:
            conf_.export(outfile, level, namespace_, name_='conf')
    def hasContent_(self):
        if (
            self.conf
            ):
            return True
        else:
            return False
    def exportLiteral(self, outfile, level, name_='complex_mbngen_conf'):
        level += 1
        self.exportLiteralAttributes(outfile, level, [], name_)
        if self.hasContent_():
            self.exportLiteralChildren(outfile, level, name_)
    def exportLiteralAttributes(self, outfile, level, already_processed, name_):
        pass
    def exportLiteralChildren(self, outfile, level, name_):
        showIndent(outfile, level)
        outfile.write('conf=[\n')
        level += 1
        for conf_ in self.conf:
            showIndent(outfile, level)
            outfile.write('model_.complex_conf(\n')
            conf_.exportLiteral(outfile, level, name_='complex_conf')
            showIndent(outfile, level)
            outfile.write('),\n')
        level -= 1
        showIndent(outfile, level)
        outfile.write('],\n')
    def build(self, node):
        self.buildAttributes(node, node.attrib, [])
        for child in node:
            nodeName_ = Tag_pattern_.match(child.tag).groups()[-1]
            self.buildChildren(child, node, nodeName_)
    def buildAttributes(self, node, attrs, already_processed):
        pass
    def buildChildren(self, child_, node, nodeName_, fromsubclass_=False):
        if nodeName_ == 'conf':
            obj_ = complex_conf.factory()
            obj_.build(child_)
            self.conf.append(obj_)
# end class complex_mbngen_conf


class complex_conf(GeneratedsSuper):
    subclass = None
    superclass = None
    def __init__(self, header_length=None, image_id=None, tag_name=None, header_version=None, image_dest_ptr=None, image_src=None):
        self.header_length = _cast(int, header_length)
        self.image_id = _cast(int, image_id)
        self.tag_name = _cast(None, tag_name)
        self.header_version = _cast(int, header_version)
        self.image_dest_ptr = _cast(int, image_dest_ptr)
        self.image_src = _cast(int, image_src)
        pass
    def factory(*args_, **kwargs_):
        if complex_conf.subclass:
            return complex_conf.subclass(*args_, **kwargs_)
        else:
            return complex_conf(*args_, **kwargs_)
    factory = staticmethod(factory)
    def get_header_length(self): return self.header_length
    def set_header_length(self, header_length): self.header_length = header_length
    def get_image_id(self): return self.image_id
    def set_image_id(self, image_id): self.image_id = image_id
    def get_tag_name(self): return self.tag_name
    def set_tag_name(self, tag_name): self.tag_name = tag_name
    def get_header_version(self): return self.header_version
    def set_header_version(self, header_version): self.header_version = header_version
    def get_image_dest_ptr(self): return self.image_dest_ptr
    def set_image_dest_ptr(self, image_dest_ptr): self.image_dest_ptr = image_dest_ptr
    def get_image_src(self): return self.image_src
    def set_image_src(self, image_src): self.image_src = image_src
    def export(self, outfile, level, namespace_='tns:', name_='complex_conf', namespacedef_=''):
        showIndent(outfile, level)
        outfile.write('<%s%s%s' % (namespace_, name_, namespacedef_ and ' ' + namespacedef_ or '', ))
        already_processed = []
        self.exportAttributes(outfile, level, already_processed, namespace_, name_='complex_conf')
        if self.hasContent_():
            outfile.write('>\n')
            self.exportChildren(outfile, level + 1, namespace_, name_)
            outfile.write('</%s%s>\n' % (namespace_, name_))
        else:
            outfile.write('/>\n')
    def exportAttributes(self, outfile, level, already_processed, namespace_='tns:', name_='complex_conf'):
        if self.header_length is not None and 'header_length' not in already_processed:
            already_processed.append('header_length')
            outfile.write(' header_length="%s"' % self.gds_format_integer(self.header_length, input_name='header_length'))
        if self.image_id is not None and 'image_id' not in already_processed:
            already_processed.append('image_id')
            outfile.write(' image_id="%s"' % self.gds_format_integer(self.image_id, input_name='image_id'))
        if self.tag_name is not None and 'tag_name' not in already_processed:
            already_processed.append('tag_name')
            outfile.write(' tag_name=%s' % (self.gds_format_string(quote_attrib(self.tag_name).encode(ExternalEncoding), input_name='tag_name'), ))
        if self.header_version is not None and 'header_version' not in already_processed:
            already_processed.append('header_version')
            outfile.write(' header_version="%s"' % self.gds_format_integer(self.header_version, input_name='header_version'))
        if self.image_dest_ptr is not None and 'image_dest_ptr' not in already_processed:
            already_processed.append('image_dest_ptr')
            outfile.write(' image_dest_ptr="%s"' % self.gds_format_integer(self.image_dest_ptr, input_name='image_dest_ptr'))
        if self.image_src is not None and 'image_src' not in already_processed:
            already_processed.append('image_src')
            outfile.write(' image_src="%s"' % self.gds_format_integer(self.image_src, input_name='image_src'))
    def exportChildren(self, outfile, level, namespace_='tns:', name_='complex_conf', fromsubclass_=False):
        pass
    def hasContent_(self):
        if (

            ):
            return True
        else:
            return False
    def exportLiteral(self, outfile, level, name_='complex_conf'):
        level += 1
        self.exportLiteralAttributes(outfile, level, [], name_)
        if self.hasContent_():
            self.exportLiteralChildren(outfile, level, name_)
    def exportLiteralAttributes(self, outfile, level, already_processed, name_):
        if self.header_length is not None and 'header_length' not in already_processed:
            already_processed.append('header_length')
            showIndent(outfile, level)
            outfile.write('header_length = %d,\n' % (self.header_length,))
        if self.image_id is not None and 'image_id' not in already_processed:
            already_processed.append('image_id')
            showIndent(outfile, level)
            outfile.write('image_id = %d,\n' % (self.image_id,))
        if self.tag_name is not None and 'tag_name' not in already_processed:
            already_processed.append('tag_name')
            showIndent(outfile, level)
            outfile.write('tag_name = "%s",\n' % (self.tag_name,))
        if self.header_version is not None and 'header_version' not in already_processed:
            already_processed.append('header_version')
            showIndent(outfile, level)
            outfile.write('header_version = %d,\n' % (self.header_version,))
        if self.image_dest_ptr is not None and 'image_dest_ptr' not in already_processed:
            already_processed.append('image_dest_ptr')
            showIndent(outfile, level)
            outfile.write('image_dest_ptr = %d,\n' % (self.image_dest_ptr,))
        if self.image_src is not None and 'image_src' not in already_processed:
            already_processed.append('image_src')
            showIndent(outfile, level)
            outfile.write('image_src = %d,\n' % (self.image_src,))
    def exportLiteralChildren(self, outfile, level, name_):
        pass
    def build(self, node):
        self.buildAttributes(node, node.attrib, [])
        for child in node:
            nodeName_ = Tag_pattern_.match(child.tag).groups()[-1]
            self.buildChildren(child, node, nodeName_)
    def buildAttributes(self, node, attrs, already_processed):
        value = find_attr_value_('header_length', node)
        if value is not None and 'header_length' not in already_processed:
            already_processed.append('header_length')
            try:
                self.header_length = int(value)
            except ValueError as exp:
                raise_parse_error(node, 'Bad integer attribute: %s' % exp)
        value = find_attr_value_('image_id', node)
        if value is not None and 'image_id' not in already_processed:
            already_processed.append('image_id')
            try:
                self.image_id = int(value)
            except ValueError as exp:
                raise_parse_error(node, 'Bad integer attribute: %s' % exp)
        value = find_attr_value_('tag_name', node)
        if value is not None and 'tag_name' not in already_processed:
            already_processed.append('tag_name')
            self.tag_name = value
        value = find_attr_value_('header_version', node)
        if value is not None and 'header_version' not in already_processed:
            already_processed.append('header_version')
            try:
                self.header_version = int(value)
            except ValueError as exp:
                raise_parse_error(node, 'Bad integer attribute: %s' % exp)
        value = find_attr_value_('image_dest_ptr', node)
        if value is not None and 'image_dest_ptr' not in already_processed:
            already_processed.append('image_dest_ptr')
            try:
                self.image_dest_ptr = int(value)
            except ValueError as exp:
                raise_parse_error(node, 'Bad integer attribute: %s' % exp)
        value = find_attr_value_('image_src', node)
        if value is not None and 'image_src' not in already_processed:
            already_processed.append('image_src')
            try:
                self.image_src = int(value)
            except ValueError as exp:
                raise_parse_error(node, 'Bad integer attribute: %s' % exp)
    def buildChildren(self, child_, node, nodeName_, fromsubclass_=False):
        pass
# end class complex_conf


USAGE_TEXT = """
Usage: python <Parser>.py [ -s ] <in_xml_file>
"""

def usage():
    print(USAGE_TEXT)
    sys.exit(1)


def get_root_tag(node):
    tag = Tag_pattern_.match(node.tag).groups()[-1]
    rootClass = globals().get(tag)
    return tag, rootClass


def parse(inFileName):
    doc = parsexml_(inFileName)
    rootNode = doc.getroot()
    rootTag, rootClass = get_root_tag(rootNode)
    if rootClass is None:
        rootTag = 'mbngen'
        rootClass = mbngen
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
    sys.stdout.write('<?xml version="1.0" ?>\n')
    rootObj.export(sys.stdout, 0, name_=rootTag, 
        namespacedef_='xmlns:tns="http://www.qualcomm.com/mbngen"')
    return rootObj


def parseString(inString):
    from sectools.common.utils.compat import StringIO
    doc = parsexml_(StringIO(inString))
    rootNode = doc.getroot()
    rootTag, rootClass = get_root_tag(rootNode)
    if rootClass is None:
        rootTag = 'mbngen'
        rootClass = mbngen
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
    sys.stdout.write('<?xml version="1.0" ?>\n')
    rootObj.export(sys.stdout, 0, name_="mbngen",
        namespacedef_='xmlns:tns="http://www.qualcomm.com/mbngen"')
    return rootObj


def parseLiteral(inFileName):
    doc = parsexml_(inFileName)
    rootNode = doc.getroot()
    rootTag, rootClass = get_root_tag(rootNode)
    if rootClass is None:
        rootTag = 'mbngen'
        rootClass = mbngen
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
    sys.stdout.write('#from auto_gen_xml_config import *\n\n')
    sys.stdout.write('import auto_gen_xml_config as model_\n\n')
    sys.stdout.write('rootObj = model_.rootTag(\n')
    rootObj.exportLiteral(sys.stdout, 0, name_=rootTag)
    sys.stdout.write(')\n')
    return rootObj


def main():
    args = sys.argv[1:]
    if len(args) == 1:
        parse(args[0])
    else:
        usage()


if __name__ == '__main__':
    #import pdb; pdb.set_trace()
    main()


__all__ = [
    "complex_conf",
    "complex_mbngen_conf",
    "mbngen"
    ]
