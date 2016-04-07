<?xml version="1.0"?>
<!-- 
This file is designed to be included in the code verbatium. Use the make_includes.sh script to prepare for inclusion
-->
<xsl:stylesheet version="1.0"
				xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
				xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
				xmlns:xsd="http://www.w3.org/2001/XMLSchema">

	<xsl:template match="/">
		<html>
			<head>
				<style>
					body, td {
					font-family: "Helvetica", sans-serif;
					font-size:12px;
					}
					td {
					vertical-align: top;
					}
					td.rank {
					text-align: right
					}
					td.header {
					text-align: right;
					color:hsl(0,0%,60%);
					}
					tr.relevant {
					background-color:hsl(120,100%,90%);
					}
					tr.nonrelevant {
					background-color:hsl(0,100%,90%);
					}
					table, td, tr, h2, h3 {
					border:0;
					margin:0;
					padding:1px;
					}
					.identifier {
					font-size:10px;
					color:hsl(0,0%,60%);
					}
					table.numbers tr td {
					text-align: right;
					padding-left:0.5em;
					padding-right:0.5em;
					}
					h3 {
					margin-top:0.5em;
					}
					td.query {
					padding-top:1em;
					}
				</style>
			</head>
			<body>
				<h2>Classifier Test Report</h2>

				<xsl:apply-templates select="report/description"/>
				<xsl:apply-templates select="report/classifierParameters"/>
				<xsl:apply-templates select="report/result"/>

				<h3>Queries</h3>

				<table>
					<xsl:apply-templates select="report/experiments/retrieval"/>
				</table>

			</body>
		</html>
	</xsl:template>

	<xsl:template match="classifierParameters">
		<h3>Classifier</h3>
		<table class="numbers">
			<xsl:for-each select="field">
				<tr>
					<td class="header"><xsl:value-of select="@name"/></td>
					<td><xsl:value-of select="format-number(number(.), '0.0000')"/></td>
				</tr>
			</xsl:for-each>
		</table>
	</xsl:template>


	<xsl:template match="description">
		<table>
			<xsl:apply-templates select="field"/>
		</table>
	</xsl:template>

	<xsl:template match="field">
		<tr>
			<td class="header"><xsl:value-of select="@name"/></td>
			<td><xsl:value-of select="."/></td>
		</tr>
	</xsl:template>

	<xsl:template match="field[@name='date']">
		<tr>
			<td class="header"><xsl:value-of select="@name"/></td>
			<td>
				<xsl:variable name="dt" select="."/>
				<xsl:value-of select="substring($dt,6,2)"/>/<xsl:value-of select="substring($dt,9,2)"/>/<xsl:value-of select="substring($dt,1,4)"/>
				<xsl:text> </xsl:text>
				<xsl:value-of select="substring($dt,12,8)"/>
			</td>
		</tr>
	</xsl:template>

	<xsl:template match="retrieval">
		<tr>
			<td colspan="2" class="query">
				<xsl:value-of select="query/field[@name='text']"/><xsl:text> </xsl:text>
				<span class="identifier">(<xsl:value-of select="query/@id"/>,
					<xsl:value-of select="query/field[@name='id']"/>)
				</span>
			</td>
		</tr>
		<tr>
			<td colspan="2" class="identifier">
				<span >
					retrieved:
					<xsl:value-of select="format-number(number(result/field[@name='retrievedCount']), '##')"/>;
					relevant:
					<xsl:value-of select="format-number(number(result/field[@name='relevantCount']), '##')"/>
					of
					<xsl:value-of select="format-number(number(result/field[@name='relevantSetSize']), '##')"/>;
					precision:
					<xsl:value-of select="format-number(number(result/field[@name='precision']), '0.00')"/>;
					recall:
					<xsl:value-of select="format-number(number(result/field[@name='recall']), '0.00')"/>;
					F-Score:
					<xsl:value-of select="format-number(number(result/field[@name='fScore']), '0.00')"/>
				</span>
			</td>
		</tr>
		<xsl:for-each select="rankedList/utterance">
			<tr>
				<xsl:attribute name="class">
					<xsl:choose>
						<xsl:when test="@relevant='true'">
							<xsl:text>relevant</xsl:text>
						</xsl:when>
						<xsl:otherwise>
							<xsl:text>nonrelevant</xsl:text>
						</xsl:otherwise>
					</xsl:choose>
				</xsl:attribute>

				<td class="rank">
					<xsl:value-of select="@rank"/>
				</td>
				<td>
					<xsl:value-of select="field[@name='text']"/><xsl:text> </xsl:text>
					<span class="identifier">(<xsl:value-of
						select="format-number(number(@score), '0.000')"/>,<xsl:text> </xsl:text>
						<xsl:value-of select="field[@name='id']"/>)
					</span>
				</td>
			</tr>
		</xsl:for-each>
	</xsl:template>

	<xsl:template match="result">

		<h3>Statistics</h3>

		<table class="numbers">
			<tr>
				<td class="header"># of queries</td>
				<td>
					<xsl:value-of select="format-number(number(field[@name='count']), '#')"/>
				</td>
				<td/>
			</tr>
			<tr>
				<td/>
				<td>Macro</td>
				<td>Micro</td>
			</tr>
			<tr>
				<td class="header"># to retrieve</td>
				<td>
					<xsl:value-of select="format-number(number(field[@name='mean_collectionSize']), '#')"/>
				</td>
				<td>
					<xsl:value-of select="format-number(number(field[@name='sum_collectionSize']), '#')"/>
				</td>
			</tr>
			<tr>
				<td class="header"># of relevant to retrieve</td>
				<td>
					<xsl:value-of select="format-number(number(field[@name='mean_relevantSetSize']), '0.0000')"/>
				</td>
				<td>
					<xsl:value-of select="format-number(number(field[@name='sum_relevantSetSize']), '#')"/>
				</td>
			</tr>
			<tr>
				<td class="header"># retrieved</td>
				<td>
					<xsl:value-of select="format-number(number(field[@name='mean_retrievedCount']), '0.0000')"/>
				</td>
				<td>
					<xsl:value-of select="format-number(number(field[@name='sum_retrievedCount']), '#')"/>
				</td>
			</tr>
			<tr>
				<td class="header"># of relevant retrieved</td>
				<td>
					<xsl:value-of select="format-number(number(field[@name='mean_relevantCount']), '0.0000')"/>
				</td>
				<td>
					<xsl:value-of select="format-number(number(field[@name='sum_relevantCount']), '#')"/>
				</td>
			</tr>
			<tr>
				<td class="header"># of non-relevant retrieved</td>
				<td>
					<xsl:value-of select="format-number(number(field[@name='mean_nonRelevantCount']), '0.0000')"/>
				</td>
				<td>
					<xsl:value-of select="format-number(number(field[@name='sum_nonRelevantCount']), '#')"/>
				</td>
			</tr>

			<tr>
				<td class="header">precision</td>
				<td>
					<xsl:value-of select="format-number(number(field[@name='macro_average_precision']), '0.0000')"/>
				</td>
				<td>
					<xsl:value-of select="format-number(number(field[@name='micro_average_precision']), '0.0000')"/>
				</td>
			</tr>
			<tr>
				<td class="header">recall</td>
				<td>
					<xsl:value-of select="format-number(number(field[@name='macro_average_recall']), '0.0000')"/>
				</td>
				<td>
					<xsl:value-of select="format-number(number(field[@name='micro_average_recall']), '0.0000')"/>
				</td>
			</tr>
			<tr>
				<td class="header">F-Score</td>
				<td>
					<xsl:value-of select="format-number(number(field[@name='macro_average_fScore']), '0.0000')"/>
				</td>
				<td>
					<xsl:value-of select="format-number(number(field[@name='micro_average_fScore']), '0.0000')"/>
				</td>
			</tr>

			<tr>
				<td class="header">P(miss)</td>
				<td>
					<xsl:value-of select="format-number(number(field[@name='macro_average_pMiss']), '0.0000')"/>
				</td>
				<td>
					<xsl:value-of select="format-number(number(field[@name='micro_average_pMiss']), '0.0000')"/>
				</td>
			</tr>
			<tr>
				<td class="header">P(false alarm)</td>
				<td>
					<xsl:value-of select="format-number(number(field[@name='macro_average_pFalseAlarm']), '0.0000')"/>
				</td>
				<td>
					<xsl:value-of select="format-number(number(field[@name='micro_average_pFalseAlarm']), '0.0000')"/>
				</td>
			</tr>

			<tr>
				<td class="header">accuracy</td>
				<td>
					<xsl:value-of select="format-number(number(field[@name='macro_average_accuracy']), '0.0000')"/>
				</td>
				<td/>
			</tr>
			<tr>
				<td class="header">average precision</td>
				<td>
					<xsl:value-of select="format-number(number(field[@name='macro_average_averagePrecision']), '0.0000')"/>
				</td>
				<td/>
			</tr>

		</table>

	</xsl:template>

</xsl:stylesheet>
