var host = document.domain;

if (host === "www.citrix.com" || host === "support.citrix.com" || host === "kb.citrite.net" ) {
	document.write('<link href="//www.citrix.com/static/kc/prod/styles.css?ver=1.3" rel="stylesheet" type="text/css" />');
} else {
	document.write('<link href="//www.citrix.com/static/kc/qa/styles.css?ver=1.3" rel="stylesheet" type="text/css" />');
}

if (host === "www.citrix.com" || host === "support.citrix.com" ) {
	//document.write('<script language="javaScript" src="//www.citrix.com/static/kc/prod/autosupport/autoSupportScripts.js?ver=1.2" type="text/javascript"></script>');
} else if ( host === "kb.citrite.net" ) {
	
} else if ( host === "localhost.citrix.com" ) {
	
} else if ( host === "supportqa.citrix.com" ) {
	
}
else {
	
}
