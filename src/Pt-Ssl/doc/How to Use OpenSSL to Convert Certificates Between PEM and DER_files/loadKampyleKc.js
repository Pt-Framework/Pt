var host = document.domain;

if (host === "support.citrix.com") {
	document.write('<script language="JavaScript" src="//www.citrix.com/static/kampyle/prod/collapseFeedbackKc.js"></script> '); 
	document.write('<script language="JavaScript" src="//www.citrix.com/static/kampyle/prod/k_config_3961704.js"></script> '); 
	document.write('<link href="//www.citrix.com/static/kampyle/prod/css/layoutSupportMods.css" rel="stylesheet" type="text/css" />');	
} else if ( host === "supportqa.citrix.com" ) {
	document.write('<script language="JavaScript" src="//www.citrix.com/static/kampyle/qa/k_config_3961704.js"></script> '); 
	document.write('<link href="//www.citrix.com/static/kampyle/qa/css/layoutSupportsModsVertical.css" rel="stylesheet" type="text/css" />');	

	window.onload=function() {
		feedbackState(); 
	}

	function feedbackState() {
		document.getElementById("kampylink").className = 'k_float k_bottom k_right feedBackInit';
	}


} else if ( host === "localhost.citrix.com" ) {
	document.write('<script language="JavaScript" src="//www.citrix.com/static/kampyle/qa/k_config_3961704.js"></script> '); 
	document.write('<link href="//www.citrix.com/static/kampyle/qa/css/layoutSupportMods.css" rel="stylesheet" type="text/css" />');	
	
	window.onload=function() {
		feedbackState(); 
	}

	function feedbackState() {
		document.getElementById("kampylink").className = 'k_float k_bottom k_right feedBackInit';
	}
	
} else {
	document.write('<script language="JavaScript" src="//www.citrix.com/static/kampyle/qa/k_config_3961704.js"></script> '); 
	document.write('<link href="//www.citrix.com/static/kampyle/qa/css/layoutSupportMods.css" rel="stylesheet" type="text/css" />');	
	
	window.onload=function() {
		feedbackState(); 
	}

	function feedbackState() {
		document.getElementById("kampylink").className = 'k_float k_bottom k_right feedBackInit';
	}
	
}



	        
