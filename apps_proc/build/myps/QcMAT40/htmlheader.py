"""
 htmlheader.py

Description: This file is used for HTML Report Generation.
             Contains links to sorting scripts and controls
             how table should display

 Copyright (c) 2016 - 2017 by QUALCOMM, Incorporated.  
             All Rights Reserved.

========================================================================

 EDIT HISTORY FOR MODULE

 $Header: htmlheader.py

 when            who        what, where, why
 --------      ---------  ------------------------------------------------
 July 7, 2016                 First Draft
 ==========================================================================
 """

htmlcssheader = """
<html>
<head>
<SCRIPT type=text/javascript src="http://harv-mperepa/qcmat/scripts/table.js"></SCRIPT>
<LINK rel=stylesheet type=text/css href="http://harv-mperepa/qcmat/scripts/table.css" media=all>
</head>
<body>
"""

htmldirtable = """
<table class="table-autosort:0 table-autostripe table-stripeclass:alternate" align="center" border="1" cellspacing="0" cellpadding="1">
<thead>
<tr bgcolor="#736AFF">
<th class=table-sortable:default><font color="FFFFFF">Directory</font></th>
<th class=table-sortable:numeric><font color="FFFFFF">&nbsp;&nbsp;TXT&nbsp;&nbsp;</font></th>
<th class=table-sortable:numeric><font color="FFFFFF">&nbsp;&nbsp;&nbsp;RO&nbsp;&nbsp;&nbsp;</font></th>
<th class=table-sortable:numeric><font color="FFFFFF">&nbsp;DATA&nbsp;</font></th>
<th class=table-sortable:numeric><font color="FFFFFF">&nbsp;&nbsp;BSS&nbsp;&nbsp</font></th>
<th class=table-sortable:numeric><font color="FFFFFF">&nbsp;TOTAL&nbsp</font></th>
<th class=table-sortable:numeric><font color="FFFFFF">FatalCount</font></th>
<th class=table-sortable:numeric><font color="FFFFFF">&nbsp;F3Count&nbsp</font></th>
<th class=table-sortable:numeric><font color="FFFFFF">&nbsp;UnKnown&nbsp</font></th>
</tr>
</thead>
<tbody>
"""

htmldirdifftable = """
<table class="table-autosort:0 table-autostripe table-stripeclass:alternate" align="center" border="1" cellspacing="0" cellpadding="1">
<thead>
<tr bgcolor="#736AFF">
<th class=table-sortable:default><font color="FFFFFF">Directory</font></th>
<th class=table-sortable:numeric><font color="FFFFFF">&nbsp;TXT1&nbsp;</font></th>
<th class=table-sortable:numeric><font color="FFFFFF">&nbsp;&nbsp;RO1&nbsp;&nbsp;</font></th>
<th class=table-sortable:numeric><font color="FFFFFF">&nbsp;DATA1&nbsp;</font></th>
<th class=table-sortable:numeric><font color="FFFFFF">&nbsp;BSS1&nbsp</font></th>
<th class=table-sortable:numeric><font color="FFFFFF">TOTAL1</font></th>
<th class=table-sortable:numeric><font color="FFFFFF">Fatal-1</font></th>
<th class=table-sortable:numeric><font color="FFFFFF">F3-1</font></th>
<th class=table-sortable:numeric><font color="FFFFFF">TXT diff</font></th>
<th class=table-sortable:numeric><font color="FFFFFF">RO diff</font></th>
<th class=table-sortable:numeric><font color="FFFFFF">DATA diff</font></th>
<th class=table-sortable:numeric><font color="FFFFFF">BSS diff</font></th>
<th class=table-sortable:numeric><font color="FFFFFF">TOTAL diff</font></th>
<th class=table-sortable:numeric><font color="FFFFFF">Fatal diff</font></th>
<th class=table-sortable:numeric><font color="FFFFFF">F3 Diff</font></th>
</tr>
</thead>
<tbody>
"""
