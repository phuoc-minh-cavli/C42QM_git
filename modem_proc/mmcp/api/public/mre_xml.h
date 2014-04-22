#ifndef _MRE_XML_H_
#define _MRE_XML_H_

/**
  @file mre_xml.h

  @brief  XML parser for Modem Rules Engine
*/

/*
    Copyright (c) 2013,2014 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //components/rel/mmcp.mpss/6.1.10/api/public/mre_xml.h#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/


#include "mre.h"


typedef struct mre_xml_root_t     mre_xml_root_t;


/*=============================================================================
  Public APIs to be used in code that reads the XML document tree to create
  conditions and actions.
=============================================================================*/

/*-------- mre_xml_get_tag --------*/
/**
@brief  Get the name of the tag of the given element node.

@param[in]  pElement  Pointer to an element node of the document.

@return
  Pointer to a NUL-terminated string that is the name of the document.
  Do not free this pointer.
*/
const char *mre_xml_get_tag(
  const mre_xml_element_t *pElement
);


/*-------- mre_xml_tag_is --------*/
/**
@brief  Return whether the tag of this element is that of the string
        passed in.

@param[in]  pElement  Pointer to an element node of the document.
@param[in]  pStr      Pointer to the string to compare to the tag.

@return
  TRUE if the string is the same as the tag, FALSE otherwise.
*/
boolean mre_xml_tag_is(
  const mre_xml_element_t *pElement,
  const char              *pStr
);


/*-------- mre_xml_get_attribute --------*/
/**
@brief  Find the value for a given attribute name in an element node.

@param[in]  pElement  Pointer to the element node.
@param[in]  pName     Pointer to the name of the attribute to find.

@return
  Pointer to a NUL-terminated string that is the value of the attribute if found.
  Do not free this pointer.
  NULL if the attribute is not found.
*/
const char *mre_xml_get_attribute(
  const mre_xml_element_t *pElement,
  const char              *pName
);


/*-------- mre_xml_get_text --------*/
/**
@brief  Return the text associated with an element.

@param[in]  pElement  Pointer to the element node.

@return
  Pointer to a NUL-terminated string that is the text associated with the element.
  Do not free this pointer.
  NULL if there is no text associated with the element.
*/
const char *mre_xml_get_text(
  const mre_xml_element_t *pElement
);
  

/*-------- mre_xml_num_children --------*/
/**
@brief  Return the number of child nodes of this element node.

@param[in]  pElement  Pointer to an element node of the document.

@return
  The number of children of this element node.
*/
size_t mre_xml_num_children(
  const mre_xml_element_t *pElement
);


/*-------- mre_xml_get_child --------*/
/**
@brief  Return a pointer to a child element node of this element node.

@param[in]  pElement  Pointer to an element node of the document.
@param[in]  nChild    Which child to return (zero based).

@return
  A pointer to the appropriate child element, or NULL if number of children
  of the node is smaller than the child number requested.
*/
const mre_xml_element_t *mre_xml_get_child(
  const mre_xml_element_t *pParent,
  size_t                  nChild
);


/*-------- mre_xml_parse --------*/
/**
@brief  Parse an XML document into a document tree.

@param[in]  pDocument   Pointer to the document to parse.
@param[out] ppRoot      Pointer to storage for a pointer to a reference-
                        counted document tree root.  The caller should
                        call ref_cnt_obj_release() on this pointer when it
                        no longer needs the document tree.

@return
  MRE_STATUS_SUCCESS if the document is successfully parsed.
  MRE_STATUS_ERR_MALFORMED_XML the XML is malformed.
*/
mre_status_t mre_xml_parse(
  const char            *pDocument,
  const mre_xml_root_t  **ppRoot
);


/*-------- mre_xml_parse_from_text --------*/
/**
@brief  Parse an XML document into a document tree.  This scans an arbitrary
        string (such as an HTTP response) until it finds an <?xml> tag, and
        then parses the following XML and returns a pointer to the root of the
        document tree.

@param[in]  pDocument   Pointer to the text to parse.
@param[out] ppRoot      Pointer to storage for a pointer to a reference-
                        counted document tree root.  The caller should
                        call ref_cnt_obj_release() on this pointer when it
                        no longer needs the document tree.

@return
  MRE_STATUS_SUCCESS if the document is successfully parsed.
  MRE_STATUS_ERR_MALFORMED_XML the XML is malformed.
*/
mre_status_t mre_xml_parse_from_text(
  const char            *pDocument,
  const mre_xml_root_t  **ppRoot
);


/*-------- mre_xml_get_text_from_child --------*/
/**
@brief  Return the text associated with a child element with the given tag.

@param[in]  pElem   Element in which to look for the child element.
@param[in]  pTag    Pointer to the tag for the desired child.

@return
  NULL if a child element of 'pElem' with the tag 'pTag' is not found, or
  if the child does not have any text (i.e. is of the form <pTag />.
  Otherwise, a pointer to the text associated with the child.
*/
char const *mre_xml_get_text_from_child(
  mre_xml_element_t const *pElem,
  char const              *pTag
);


/*-------- mre_xml_get_root_element --------*/
/**
@brief  Returns a pointer to the root element of the document.

@param[in]  pRoot   Pointer to the XML document tree root returned by
                    mre_xml_parse().

@return
  Pointer to the root element of the document, or NULL if there is none.
*/
const mre_xml_element_t *mre_xml_get_root_element(
  const mre_xml_root_t  *pRoot
);

#endif /* _MRE_XML_H_ */
