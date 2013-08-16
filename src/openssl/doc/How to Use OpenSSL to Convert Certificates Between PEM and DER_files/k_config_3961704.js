/* === Kampyle Core Code - Do Not Edit =====================================================================================*/
/* Console might not be defined, so use silent fallback */

var k_start = new Date();

if (typeof (console) == "undefined"){
	console = { 'log' : function(){} };
}
/* Check if tags are already on the page */
if (typeof(k_tags_created) == "undefined"){
	k_tags_created = false;
}

k_config={

	site: {},
	params: {
		"navLevel0" : "",
		"navLevel1" : "",
		"navLevel2" : "",
		"navLevel3" : "",
		"navLevel4" : "",
		"navLevel5" : "",
		"navFull" : "",
		"siteSection" : "",
		"k_error" : ""	
	},
	crumbArray: [],
	
	// INTERNAL FUNCTIONS
	
	consoleLog: function(msgText,msgLevel){
		if (typeof (msgText) === 'undefined' || msgText === 'null' || msgText === '') {
			msgText = 'NO MESSAGE';
			}
		if (k_config.site.testMode == true || k_config.site.testMode == 1 || k_config.site.testMode == msgLevel) {
			console.log('K: ' + msgText);
		}
	},
	
	loadScript: function(src,obj,callback) {
		k_config.consoleLog("loadScript() src:"+src+" obj:"+obj,99);
		if(src == ''||src == undefined||src == 'http://'||src == 'https://'){
			k_config.params.k_error += '|'+'loadScript('+src+')';
			k_config.consoleLog('loadScript return (src == '+src+')',99);
			return false;
		}
		var head = document.getElementsByTagName('head')[0],
			script = document.createElement('script');
		done = false;
		script.setAttribute('src', src);
		script.setAttribute('type', 'text/javascript');
		script.setAttribute('charset', 'utf-8');
		script.onload = script.onreadstatechange = function() {
			if (!done && (!this.readyState || this.readyState == 'loaded' || this.readyState == 'complete')) {
				done = true;
				script.onload = script.onreadystatechange = null;
					if (callback) {
						callback();
					}
				}
			if(typeof (obj) != "undefined"){
				k_config.consoleLog(obj + " loaded ok!",99);
				k_config.getParams("on "+obj+"Load");
			}if(typeof (obj) == "undefined"){
				k_config.consoleLog("ERROR LOADING " + obj + "!",99);
			}
		}
		head.insertBefore(script, head.firstChild);
	},

	loadjscssfile: function(src, filetype){
		if(src == ''||src == undefined||src == 'http://'||src == 'https://'){
			k_config.params.k_error += '|'+'loadScript('+src+')';
			k_config.consoleLog('loadScript return('+src+')');
			return false;
		}
		if (filetype == "js"){ //if filename is a external JavaScript file
			var fileref=document.createElement('script');
			fileref.setAttribute("type","text/javascript");
			fileref.setAttribute("src", src);
		}
		else if (filetype == "css"){ //if filename is an external CSS file
			var fileref=document.createElement("link");
			fileref.setAttribute("rel", "stylesheet");
			fileref.setAttribute("type", "text/css");
			fileref.setAttribute("href", src);
		}
		if (typeof fileref!="undefined"){
			document.getElementsByTagName("head")[0].appendChild(fileref);
		}
	},
	
	buttonParams: function(){
		var buttonParams = "site_code="+k_config.site.site_code+"&form_id="+k_config.site.button_form_id+"&lang="+k_config.site.lang;
		return buttonParams;
	},
	
	pushParams: function(){
		var pushParams = "site_code="+k_config.site.site_code+"&form_id="+((k_push_vars.invite_form_id)?k_push_vars.invite_form_id:k_config.site.button_form_id)+"&lang="+k_config.site.lang;
		return pushParams;
	},

	
	// ON PAGE INSTALLATION + EVENT HANDLERS

	evalPage: function(urlOrString,paramToTest){
	k_config.consoleLog('Chk: '+urlOrString);
	k_config.consoleLog('PTTin: '+paramToTest);
		try{ 
			if(paramToTest == ""||typeof (paramToTest)  == "undefined"){
				paramToTest="re";
			};
	k_config.consoleLog('PTTout: '+paramToTest);
				for (var keys in k_config.siteSections) {
					var newRE = new RegExp(k_config.siteSections[keys][paramToTest],k_config.siteSections[keys].reMods);
					k_config.consoleLog('[Keys:'+keys+'][String:'+urlOrString+']['+paramToTest+':'+k_config.siteSections[keys][paramToTest]+'][newRE:'+newRE+']');
					if(typeof (k_config.siteSections[keys]) == "function"){
						k_config.consoleLog('Key is a function - continue');
						continue;
					}
					if(typeof (k_config.siteSections[keys][paramToTest]) == "undefined"||k_config.siteSections[keys][paramToTest] == ""){
						k_config.consoleLog('Nothing to compare - continue');
						continue;
					}
						//if(urlOrString == siteSections[keys][paramToTest]) {
						if(urlOrString.match(newRE)) {
						k_config.consoleLog('MATCH!');
							try{
								k_config.loadSectionConfig(k_config.siteSections[keys],'evalPage');
								
								// Made a match!!!!!
								k_config.consoleLog('Match: '+k_config.params.siteSection+' fid: '+k_config.site.button_form_id);
								
								var ss=k_config.siteSections[keys];
								if (ss['stopOnThisMatch']!="undefined"&&ss['stopOnThisMatch'] == true){
									k_config.consoleLog('stopOnThisMatch:'+ss['stopOnThisMatch']);
									return true;
								}
							}catch(err){
								k_config.params.k_error += '|'+'evalPage ('+err+') Going to next RegEx.';
							}
					}else{
						k_config.consoleLog('No Match!');
						if (k_config.params.siteSection == "undefined"||k_config.params.siteSection == ""){
							k_config.params.siteSection = 'non-match';
							// As a precaution, we also add this as a k_button variable immediately if we can
							if (typeof (k_button)!="undefined"){
								k_button.setCustomVariable('siteSection','non-match');
							}
						}
						//return false;
					}
				}			
							
		}catch(err){
			k_config.consoleLog('Error caught on evalPage (Code: '+err+')');
			k_config.params.siteSection = 'non-match-error';
			if (typeof (k_button)!="undefined"){
				k_button.setCustomVariable('siteSection','non-match-error');
			}
			k_config.params.k_error += '|evalPage: '+err;
		}
	},
	
	createKampyleTags: function(root){
	// Note that 'Tags' created here should include <a> tags for BOTH button (kampylink) and invitation

		// First check to avoid duplication of tags on the page
		if(k_tags_created){
			k_config.consoleLog('Button already exists.  Not added again.');
			return;
		}
		
		// Load CSS
		try{
			if(typeof (k_config.site.url_css) != 'undefined'){
			k_config.consoleLog('Loading CSS: '+ k_config.site.protocolLoad+k_config.site.url_css);
				k_config.loadjscssfile(k_config.site.protocolLoad+k_config.site.url_css, "css");
			}
		}catch(err){
			k_config.consoleLog('CSS Load Error ('+err+')');
		}
		// Load k_button
		try{
			k_config.consoleLog('Loading k_button: '+ k_config.site.protocolLoad+k_config.site.url_kbutton,99);
			// Check if k_button is already loaded
			if (typeof(k_button) == "object" && typeof(k_button.open_ff)){
				k_config.consoleLog('Loading TERMINATED - k_button already on page',99)
			} else {
				k_config.loadScript(k_config.site.protocolLoad+k_config.site.url_kbutton,"k_button");
			}
		}catch(err){
			k_config.consoleLog('k_button Load Error ('+err+')',99);
		}
		// Note: Load k_push is below
		
		
		// Check if an elem was specified, otherwise use "kampylink"
		if((k_config.site.buttonID==undefined)||(typeof(k_config.site.buttonID)=="undefined")||(typeof(k_config.site.buttonID)=="")){
			k_config.site.buttonID = "kampylink";
		}
		
		// Check if we are using an existing DOM element otherwise need to add a new one
		if(document.getElementById(k_config.site.buttonID)){
			k_config.consoleLog('Binding to existing element ('+ k_config.site.buttonID +') found on page! ',6);
			var addElem = false;
		}else{
			k_config.consoleLog('Creating and Binding to new element ('+ k_config.site.buttonID +')',6);
			var addElem = true;
		}
		
		
	if(addElem){
		// Create feedback button <a> tag
		k_config.consoleLog('Adding new element: '+newElem,6);
		var aDiv = document.createElement("div");
			aDiv.id = "kampyleButton";
		var newDiv=document.body.appendChild(aDiv);
		var newText=newDiv.appendChild(document.createTextNode(""));
		var newA=newDiv.appendChild(document.createElement("a"));
		var newElem = newA;
	}else{
		var newElem = document.getElementById(k_config.site.buttonID);
		k_config.consoleLog('Using existing element: '+newElem,6);
	}
	
		if(k_config.site.formURL==undefined||typeof(k_config.site.formURL)=="undefined"||k_config.site.formURL==""){
			k_config.site.formURL = "/feedback_form/ff-feedback-form.php?";
		}
		
		if(k_config.site.buttonTarget==undefined||typeof(k_config.site.buttonTarget)=="undefined"||k_config.site.buttonTarget==""){
			k_config.site.buttonTarget = "kampyleWindow";
		}
		
		if(k_config.site.buttonID==undefined||typeof(k_config.site.buttonID)=="undefined"||k_config.site.buttonID==""){
			k_config.site.buttonID = "kampylink";
		}
		
		// Just in case k_button has completed loading before 'kampylink' was initiated on the page...
		if (typeof (k_button)!="undefined"){
				k_button.ff_link = newElem;
				k_config.site.fullQStr = k_config.getQStr(k_config.buttonParams());
				
		}else{
				k_config.site.fullQStr = k_config.buttonParams();
		}
		
		newElem.href = k_config.site.protocolForm + k_config.site.serverRegion + k_config.site.formURL + k_config.site.fullQStr;
		newElem.id = k_config.site.buttonID;
		newElem.target = k_config.site.buttonTarget;
		newElem.className = k_config.site.buttonClass
		
		//TODO: Dedup newElem.className
		
		if(k_config.site.buttonClass==undefined||typeof(k_config.site.buttonClass)=="undefined"){
			newElem.className = k_config.site.buttonClass;
		}
			
		// inviteOnly - Check if tags should be added but button not displayed
		if (k_config.site.inviteOnly == true || k_config.site.inviteOnly == "true"){
			newElem.style.display = "none";
			k_config.consoleLog('createTags - hide button. inviteOnly='+k_config.site.inviteOnly,6);
		}else{
			k_config.consoleLog('createTags - show button. inviteOnly='+k_config.site.inviteOnly,6);
		}
		
		// Moved out of with(newElem) due to bug on FF2
		newElem.onclick=function() {
			k_config.consoleLog('CLICKED here! '+newElem,6);
			k_config.handlerButtonClick();
			k_config.getParams('buttonClick');
			try {
				k_button.host_server = { value : k_config.site.serverRegion };
				if(k_config.site.useHref==true||k_config.site.useHref=="true"){
					k_config.consoleLog('CLICK: onClick Disabled (useHref=true)',6);
				}else{
					k_config.consoleLog('CLICK: Open with onClick (useHref=false)',6);
					k_button.open_ff(k_config.buttonParams());
					return false;
				}
			}catch(err){
				k_config.params.k_error += '|'+'Button onClick ('+err+')';
				k_config.consoleLog('Error Button onClick ('+err+')',6);
			}
		};		
		
	if(addElem){
		var newText = newElem.appendChild(document.createTextNode(""));
		var newImg = newElem.appendChild(document.createElement("img"));
		newImg.src = (typeof (k_config.params.url_buttonImage) == "undefined"||k_config.params.url_buttonImage == "") ? k_config.site.protocolLoad+k_config.site.url_buttonImage : k_config.site.protocolLoad+k_config.params.url_buttonImage;
		newImg.alt = "Feedback Form";
		newImg.border = "0";
	}

		// Load k_push
		if(typeof (k_push_vars.view_percentage)!="undefined"&&((k_push_vars.view_percentage>0)&&(k_push_vars.view_percentage<=100)) || (k_config.site.forceLoad_push!="undefined"&&k_config.site.forceLoad_push == "true")){
			k_config.consoleLog('Loading k_push: '+ k_config.site.protocolLoad+k_config.site.url_kpush);

			try {
				// Check if k_push is already loaded
				if (typeof(k_push) == "object" && typeof(k_push.openPopup)){
					k_config.consoleLog('Loading TERMINATED - k_push already on page',99)
				} else {
					k_config.loadScript(k_config.site.protocolLoad+k_config.site.url_kpush,"k_push");
				}
			} catch(err) {
				k_config.consoleLog('k_push Load Error ('+err+')',99);
			}
					 
				var newPushA=newDiv.appendChild(document.createElement("a"));
					
				newPushA.href = "#";
				newPushA.target = k_config.site.buttonTarget;
				newPushA.id = "k_popup_link";
				newPushA.style.display = "none";
					
				newPushA.onclick=function() {
					// Hint: Use k_push.js event handlers for click tracking
					k_config.getParams('inviteClick');
					try {
						k_button.host_server = { value : k_config.site.serverRegion };
						k_button.open_ff(k_config.pushParams());
						return false;
					}catch(err){
						k_config.params.k_error += '|'+'Invite onClick ('+err+')';
						k_config.consoleLog('Error Invite onClick ('+err+')');
					}
				};
		}

		// If we got this far then tags were added without a problem.
		k_tags_created = true;
		
		//k_config.consoleLog('Button added host:'+ k_config.site.serverRegion +' sc:'+k_config.site.site_code+' fid:'+k_config.site.button_form_id+' l:'+k_config.site.lang+' section:'+k_config.params.siteSection);
		k_config.consoleLog('\n\n===== Button added ========================================\n'+'server: '+k_config.site.serverRegion +'\n'+'siteid: '+k_config.site.site_code +'\n'+'formid: '+k_config.site.button_form_id +'\n'+'langua: '+k_config.site.lang +'\n'+'ssectn: '+k_config.params.siteSection +'\n'+'===========================================================\n\n\n');
		
	},

	handlerButtonClick: function(){
	// Default handler function.  Can be overwritten in client code.
	k_config.consoleLog('Default handlerButtonClick()');
	},
	
	bindReady: function(func){
	// Emulates the "onready" event, code adapted from jQuery

		// Mozilla, Opera and webkit nightlies currently support this event
		if ( document.addEventListener ) {
			k_config.consoleLog('bindReady available for: Moz,Opera,Webkit');
			// Use the handy event callback
			document.addEventListener( "DOMContentLoaded", function(){
					document.removeEventListener( "DOMContentLoaded", arguments.callee, false );
					func();
			}, false );
		return;

		// If IE event model is used
		} else if ( document.attachEvent ) {
			k_config.consoleLog('bindReady available for: IE');
			// ensure firing before onload,
			// maybe late but safe also for iframes
			document.attachEvent("onreadystatechange", function(){
					if ( document.readyState === "complete" ) {
							document.detachEvent( "onreadystatechange", arguments.callee );
							func();
					}
			})

			// If IE and not an iframe
			// continually check to see if the document is ready
			if ( document.documentElement.doScroll && window == window.top ) ( function(){
					k_config.consoleLog('bindReady available for: IE (non-iFrame)');
					
					try {
							// If IE is used, use the trick by Diego Perini
							// http://javascript.nwbox.com/IEContentLoaded/
							document.documentElement.doScroll("left");
					} catch( error ) {
							setTimeout( arguments.callee, 0 );
							return;
					}

					// and execute any waiting functions
					func();
			})();
		return;
		}

		// A fallback to window.onload, that will always work
		if (window.onload)
		{
		k_config.consoleLog('bindReady browser detection failed - using onload');
		var oldFunc = window.onload;
		window.onload = function() { oldFunc(); func();}
		}
		else
		{
		window.onload = func;
		}
		k_config.consoleLog('bindReady - reached end of function!');
	},

	
	// DATA MANIPULATION + MANAGEMENT
	
	getFriendly: function(unfriendly){
		k_config.consoleLog('getFriendly() - checking: ['+unfriendly+']');

		for (var prop in k_config.friendlyNames) {
			var tf = typeof(k_config.friendlyNames[prop]);
			if (tf != "function" && tf != "object"){
				if (prop == unfriendly){
					var friendly = k_config.friendlyNames[prop];
					k_config.consoleLog('Match on ['+prop+']!')
					k_config.consoleLog('Renaming to: ['+k_config.friendlyNames[prop]+'] <<======================'+'\n');
					return friendly;
				}else{
					k_config.consoleLog('No match on: ['+prop+']\n');	
				}
			}
		}
		return unfriendly;
	},

	setCustomVariables: function(id,value){
		try{
			if (typeof (k_button) == "undefined"){return;}
			
			// Check for Button/k_config vers
			k_config.params.buttonRev = "KB:" + ((typeof (k_button_js_revision) == "undefined") ? "0" : k_button_js_revision.replace(/\s+|\$|Rev:/g, '')) + "|" + "KP:" + ((typeof (k_push_js_revision) == "undefined") ? "0" : k_push_js_revision.replace(/\s+|\$|Rev:/g, '')) + "|" + k_config.site.site_shortName+"|"+'KC:'+k_config.site.revision;	
			
			for (var prop in k_config.params) {
					if (prop !== "" && k_config.params[prop] !== "" && typeof (k_config.params[prop]) != "object" && typeof (k_config.params[prop]) != "function" && typeof (k_config.params[prop]) != "array" && k_config.params[prop] !== ";" && k_config.params[prop] !== "empty" && prop != "re" && prop !== "reMods"){
					if (typeof (k_button)!="undefined"){
						var n = k_config.getFriendly(prop);
						var v = k_config.params[prop];
						var vt = k_config.trim(v);
						k_button.setCustomVariable(n,vt);
					}
				}
			}
		}catch(err){
			k_config.consoleLog('Error caught setting cvs (Code: [#1] '+err+')');
			k_config.params.k_error += '|'+err;
		}
	},

	trim: function(str) {
	k_config.consoleLog('"' + str + '" : ' +  typeof(str));
	if(str==""||typeof(str)!="string") { return str }
		str = str.replace(/^\s+/, '');
		for (var i = str.length - 1; i >= 0; i--) {
			if (/\S/.test(str.charAt(i))) {
				str = str.substring(0, i + 1);
				break;
			}
		}
		return str;
	},	

	tidyPrice: function(amt,roundTo,decimalPts){
		if (!amt) { return }
		if (typeof(amt) == "string") {
			amt = Math.round(amt.replace('$','')*1);
		}
		if (typeof(amt) == "number" && typeof(roundTo) == "number") {
			amt =  Math.round(amt / roundTo) * roundTo;
		}
		if (typeof(amt) == "number" && typeof(decimalPts) == "number") {
			amt = amt.toFixed(decimalPts);
		}
		return amt;
	},
	
	sortOrder: function(key,newEntry,divider){
		if(divider == "" || divider == undefined){
			divider = ";"
		}
		var t = k_config.params[key].split(divider);
		if(newEntry != "" && newEntry != undefined){
			t.push(newEntry);
		}
		t.sort();
		t = t.join(divider);
		return t;
	},

	remove: function(){
		delete k_config;
		delete k_button;
		delete k_push;
		delete k_push_vars;
		k_tags_created=0;
		var b = (document.getElementById('kampyleButton')) ? document.getElementById('kampyleButton') : document.getElementById('kampylink');
		var p = b.parentNode;
		p.removeChild(b);
	},

	// VISITOR DATA + PARAMETERS + VISITOR TRACKING
	
	Set_Cookie: function (name, value, expires, path, domain, secure){
	// From k_button.js
		var today = new Date();
		today.setTime(today.getTime());
		if (expires) expires = expires * 1000 * 60 * 60 * 24;
		var expires_date = new Date(today.getTime() + (expires));
		document.cookie = name + "=" + escape(value) + ((expires) ? ";expires=" + expires_date.toGMTString() : "") + ((path) ? ";path=" + path : "") + ((domain) ? ";domain=" + domain : "") + ((secure) ? ";secure" : "");
	},
	
	Get_Cookie: function (name){
	// From k_button.js
		var start = document.cookie.indexOf(name + "=");
		var len = start + name.length + 1;
		if ((!start) && (name != document.cookie.substring(0, name.length))) return null;
		if (start == -1) return null;
		var end = document.cookie.indexOf(";", len);
		if (end == -1) end = document.cookie.length;
		return unescape(document.cookie.substring(len, end));
	},
	
	getFilename: function(u){ 
		var k_url=u.substring(0,(u.indexOf('#') == -1)?u.length:u.indexOf('#'));
		k_url=k_url.substring(0,(k_url.indexOf('?') == -1)?k_url.length:k_url.indexOf('?'));
		return k_url.substring(k_url.lastIndexOf('/')+1,k_url.length).replace(/(.html|.htm|.php|.aspx|.asp|.jsp|.jspx|.cfm)/g,'');
	},

	loadSectionConfig: function(obj,src){
		k_config.consoleLog('loadSectionConfig from: '+src);
		if ( obj == "" || typeof (obj) == "undefined"){
			k_config.consoleLog('loadSectionConfig undefined so set to k_pageParams');
			if (typeof (k_pageParams) != "undefined"){
				obj = k_pageParams;
			}else{
				k_config.consoleLog('loadSectionConfig - k_pageParams not found');
				//k_config.params.k_error += '| k_pageParams not found';
				return;
			}
		}
		try{
			k_config.params.pageOverride = typeof (obj);
			var m=obj;
			// Traverse the matching siteSection and assign to the appropriate object
			for (var keys in m) {
				k_config.consoleLog('[Keys:'+keys+']:['+m[keys]+'][Type:'+typeof (m[keys])+']');
				// invite to k_push_vars
				if (keys.toLowerCase() == "invite" && typeof (m[keys]) == "object"){
				var i=m[keys];
					for (var keys in i) {
						if(i[keys]!=""||typeof (i[keys])!="string"){
							k_push_vars[keys]=""+i[keys];
						}
					}
				}
				// button to k_config.site
				else if (keys.toLowerCase() == "button" && typeof (m[keys])!="string"){
				var b=m[keys];
					for (var keys in b) {
					
						k_config.consoleLog('keys: '+keys+' typeof:'+typeof(b[keys]),5);
						if((typeof(b[keys])=="string")&&(b[keys]=="")){
							k_config.consoleLog('OUT!! \nKey:' + keys + '\nOld:'+k_config.site[keys]+'\nNew:'+b[keys],5);
						} else {
							k_config.consoleLog('IN!! \nKey:' + keys + '\nOld:'+k_config.site[keys]+'\nNew:'+b[keys],5);
							k_config.site[keys]=""+b[keys];
						}
					}
				}
				// everything else to params
				else if (m[keys]!=""){
					k_config.params[keys]=""+m[keys];
				}
			}				
			
			// Made a match!!!!!
			k_config.consoleLog('loadSectionConfig Updated: '+JSON.stringify(obj));
			
		}catch(err){
			k_config.params.k_error += '|'+'loadSectionConfig error ('+err+')';
		}
	},

	getParams: function(src){
	// Default function.  Can be overwritten in client code.
	k_config.consoleLog('Default getParams('+src+')');
		if (typeof (k_button)!="undefined"){
			k_config.setCustomVariables();
		}
	},
	
	getParamsOnRun: function(src){
	// Default function.  Can be overwritten in client code.
	k_config.consoleLog('Default getParamsOnRun('+src+')');
		if (typeof (k_button)!="undefined"){
			k_config.setCustomVariables();
		}
	},
	
	getMeta: function(attrValue,valToReturn,attrName) {
		if(attrName==""||attrName==undefined){attrName="name"}
		if(valToReturn==""||valToReturn==undefined){valToReturn="content"}
		var m = document.getElementsByTagName('META');
		var i;
		for (i = 0; i < m.length; i++){
			if (m[i].getAttribute(attrName) == attrValue){
				break;
			}
		}
	  var v = m[i].getAttribute(valToReturn);
	  return v;
	},
	
	setPathLevels: function(p){
		if (typeof(p) == "undefined"||p == "undefined"||p == ""){
			return "NoPath";
		}else{
			var path = p.split("/");
			for (i=1;i<path.length;i++) {
				k_config.params["pathLevel"+i] = path[i];
			}
		}
	},
	
	getQStr: function(formParams){
	
		var myLongURL = (k_button.create_ff_url(formParams,""));
		var urlOrStr = myLongURL.shortUrl;

			if (urlOrStr.indexOf("?")<0){
				return "NoQStr";
			}else{
				var spl = urlOrStr.substring(urlOrStr.indexOf("?")+1);
				return spl;
			}
	},
	
	setAllQStr: function(urlOrStr,obj,prefix){
	if (!obj) { var obj = k_config.params; }
	var p = (typeof(prefix) == 'undefined') ? '' : prefix;
		if (urlOrStr.indexOf("?")<0){
			return "NoQStr";
		}else{
		var spl =  urlOrStr.substring(urlOrStr.indexOf("?")+1).split("&");
			for (i=0;i<spl.length;i++) {
				var pair = spl[i].split("=");
				obj[p + pair[0]] = pair[1];
			}
		}
	},
	
	getOnPageParams: function(stringObject,name){
	
		var t = typeof (stringObject);
		if (t == 'undefined'){return;}
		var n = (typeof(name) == 'undefined') ? '' : name;

		k_config.consoleLog('getPageParams('+stringObject+') Type: ['+t+']');
		
		switch(t)
		{
			case 'object':
				try{
					for (var prop in stringObject) {
						pair = prop.split("=");
						k_config.params[n + pair[0]] = stringObject[pair[0]];
						k_config.consoleLog('getPageParams adding: ['+pair[0]+' = '+stringObject[pair[0]]+']');
					}
				}catch(err){
					k_config.consoleLog('getOnPageParams('+stringObject+')['+t+'](Code: '+err+')');
					k_config.params.k_error += '|'+err;
				}
				break;
			case 'string':
				try{
					if (n == ''){return;}
					k_config.params[n] = stringObject;
					k_config.consoleLog('getPageParams adding: ['+n+' = '+stringObject+']');
				}catch(err){
					k_config.consoleLog('getOnPageParams('+stringObject+')['+t+'](Code: '+err+')');
					k_config.params.k_error += '|'+err;
				}
				break;			
			default:
				k_config.consoleLog('getPageParams('+stringObject+') DEFAULT CASE');
				break;
		}  
	
	},


	getCookieList: function(prefix){
		var prefix = typeof(prefix)!="undefined" ? prefix : "" ;
		if  (typeof(k_config.cookieList) != "undefined") {
			k_config.consoleLog('Fetching CookieList!');
			var cl = k_config.cookieList;
			for (c=0;c<cl.length;c++) {
				if (cl[c] != null && cl[c] != ""){
					var cv = k_config.Get_Cookie(cl[c]);
					k_config.params[prefix + cl[c]] = cv;
					k_config.consoleLog('CookieList['+c+'] '+cl[c]+' : '+cv);
				}else{
					k_config.consoleLog('CookieList['+c+'] cookieName "'+cl[c]+'" not valid');
				}
			}
		} else {
			k_config.consoleLog('No Cookies!');
		}
	},

	getCrumbs: function(crumbsNode,thisNode){
		try {
			function iter8(node){
				var nodes=node.childNodes;
				var len=nodes.length;
				k_config.consoleLog('getCrumbs: Nodes found:'+len);
				var c=0;
				for(var a=0;a<len;a++){
					if(nodes[a].nodeType==3){
						if(!nodes[a].nodeValue.match(/^[\s]*$/)){
							var val=nodes[a].nodeValue.replace(/(\||&amp;| |\'|\r\n|\n|\r|[\s\xA0])/g,'');
							k_config.consoleLog('getCrumbs: Node found: '+val);
							k_config.crumbArray.push(val);
						}
					}
					else{
						if(nodes[a].nodeName.toLowerCase()!="script"){
							iter8(nodes[a])
						}
					}
				}
			}
			iter8(crumbsNode);
			if (thisNode){
				k_config.consoleLog('getCrumbs: returning this:');
				k_config.consoleLog(k_config.crumbArray[thisNode]);
				return k_config.crumbArray[thisNode];
			}
		}catch(err){
			k_config.consoleLog('Breadcrumb error (Code: '+err+')',1);
			k_config.params.k_error += '|'+err;
		}
	},

	getBreadcrumbs: function(elem){
		
		if (elem==""||elem==undefined){ return }
		
		k_config.crumbArray = [];
		
		k_config.params.navFull = "noBreadcrumbs";

		if (document.getElementById(elem)){
						var cr = document.getElementById(elem);
						// Call the function with extracts and converts all the steps in the breadcrumb string
						k_config.getCrumbs(cr);
						// Iterate through the k_config.crumbArray array and clean up the entries – for example removing characters such as ">" 
						for (var i= k_config.crumbArray.length-1; i>=0; i--) {
										if (k_config.crumbArray[i] === ">") {
														k_config.crumbArray.splice(i, 1);
										}
						}
						// Remove first entry "Vousêtesici:", "YouAreHere" etc which is not normally part of the breadcrumbs
						k_config.crumbArray[0]="";
		}

		if ((typeof(k_config.crumbArray[1])!="undefined")){

						// Only change the following code you want to customize the syntax of the k_config.params.navFull string
						k_config.params.navFull = ""
						+((typeof(k_config.crumbArray[0])!="undefined")&&(k_config.crumbArray[0]!="")?k_config.crumbArray[0]:"")
			+((typeof(k_config.crumbArray[0])!="undefined")&&(typeof(k_config.crumbArray[1])!="undefined")&&( k_config.crumbArray[0]!="")?":":"")
						+((typeof(k_config.crumbArray[1])!="undefined")? k_config.crumbArray[1]:"")
						+((typeof(k_config.crumbArray[2])!="undefined")?":"+ k_config.crumbArray[2]:"")
						+((typeof(k_config.crumbArray[3])!="undefined")?":"+ k_config.crumbArray[3]:"")
						+((typeof(k_config.crumbArray[4])!="undefined")?":"+ k_config.crumbArray[4]:"")
						+((typeof(k_config.crumbArray[5])!="undefined")?":"+ k_config.crumbArray[5]:"")
						+((typeof(k_config.crumbArray[6])!="undefined")?":"+ k_config.crumbArray[6]:"")
		}

		// Set NavLevels based on breadcrumbs – starting at NavLevel1 as NavLevel0 is reserved for domain
		if ((typeof(k_config.crumbArray[1])!="undefined")){
						k_config.params.navLevel1 = (typeof(k_config.crumbArray[1])!="undefined")?k_config.crumbArray[1]:"";
						k_config.params.navLevel2 = (typeof(k_config.crumbArray[2])!="undefined")?k_config.crumbArray[2]:"";
						k_config.params.navLevel3 = (typeof(k_config.crumbArray[3])!="undefined")?k_config.crumbArray[3]:"";
						k_config.params.navLevel4 = (typeof(k_config.crumbArray[4])!="undefined")?k_config.crumbArray[4]:"";
						k_config.params.navLevel5 = (typeof(k_config.crumbArray[5])!="undefined")?k_config.crumbArray[5]:"";
		}
	},
	searchEngine: function(){
		var seParams = {};
		var se = [['google','Google','q'],['yahoo.com','Yahoo','p'],['bing.com','Bing','q'],['bing.com','Live','q'],['msn.com','MSN','q'],['aol.com','AOL','q'],['aol.com','AOL','query'],['aol.com','AOL','encquery'],['lycos.com','Lycos','query'],['ask.com','Ask','q'],['altavista.com','Altavista','q'],['search.netscape.com','Netscape','query'],['cnn.com/SEARCH','CNN','query'],['about.com','About','terms'],['baidu.com','Baidu','wd'],['dogpile.com','Dogpile','q'],['mywebsearch.com','MyWebSearch','searchfor'],['search.com','Search.com','q'],['yandex.com','Yandex','text'],['rambler.ru','Rambler','words'],['webcrawler.com','Webcrawler','q'],['excite.com','Excite','q'],['gigablast.com','Gigablast','q'],['voila.fr','Voila','rdata'],['mamma.com','Mama','query'],['mamma.com','Mamma','query'],['alltheweb.com','Alltheweb','q'],['search.virgilio.it','Virgilio','qs'],['naver.com','Naver','query'],['alice.com','Alice','qs'],['najdi.org.mk','Najdi','q'],['eniro.se','Eniro','search_word'],['seznam.cz','Seznam','q'],['wp.pl','Wirtulana Polska','szukaj'],['online.onetcenter.org','O*NET','qt'],['szukacz.pl','Szukacz','q'],['yam.com','Yam','k'],['pchome.com','PCHome','q'],['kvasir.no','Kvasir','q'],['sesam.no','Sesam','q'],['ozu.es','Ozu','q'],['terra.com','Terra','query'],['mynet.com','Mynet','q'],['ekolay.net','Ekolay','q'],['daum.net','Daum','q'],['yandex.ru','Yandex','text'],['info.com','Info.com','q'],['softonic.com','Softonic','q']];
		var l = se.length;
		var dr = document.referrer;
		for(var loop = 0;loop < l;loop++) {
			k_config.consoleLog('Check: ['+loop+'] '+se[loop][1]);
			if (dr.indexOf(se[loop][0]) > -1) {
				k_config.consoleLog('Match: ['+loop+'] '+se[loop][0]);
				k_config.params.refSearchEng = se[loop][1];
				// Set this to k_config.params instead of seParams to get ALL search engine referrer parameters
				k_config.setAllQStr(dr,seParams);
					if (typeof(seParams[se[loop][2]]) == "undefined"){
						k_config.consoleLog("query string not found");
						continue;
					}else{
						k_config.params.refSearchQ = seParams[se[loop][2]].replace(/\+|%20/g,' ');
						// Detects the 'fr' parameter sometimes sent with Yahoo to indicate the originating search engine (brand) e.g. altavista
						if (typeof(seParams['fr']) != "undefined" & seParams['fr'] != ""){
							//k_config.params.refSearchEng += ' ('+seParams["fr"]+')';
							k_config.params.refSearchEng2 = seParams["fr"];
						}
						break;
					}
			}
		}
		k_config.params.refGclid = (window.location.href) ? (window.location.href.indexOf('gclid=')>-1?true:false) : "NoURL";
	},

	getGAcookies: function(cookieObj){
		gaParams = {};
		var o = (typeof(cookieObj)=="undefined")?gaParams:cookieObj;
		if (k_config.Get_Cookie("__utmz") !== null) {
			o._utmz = k_config.Get_Cookie("__utmz");
			o._utma = k_config.Get_Cookie("__utma");
			o._utmv = k_config.Get_Cookie("__utmv");
		
			var utmz = o._utmz.split("|");
			var utmz0 = utmz[0].split(".");
			//k_config.params.utmz_domain_hash=utmz0[0];
			k_config.params.utmz_timestamp=utmz0[1];
			k_config.params.utmz_numberOfSessions=utmz0[2];
			k_config.params.utmz_visitSources=utmz0[3];
			//k_config.params.utmz_campaign=utmz0[4];
				var cd = utmz0[4].split("=");
				k_config.params[cd[0]] = cd[1];
				
			for (i=1;i<utmz.length;i++) {
				var pair = utmz[i].split("=");
				k_config.params[pair[0]] = pair[1];
			}
		}
	},

	getTimeOnSite: function(){
		if(typeof(k_config.params.utmz_timestamp)=='undefined' && k_config.Get_Cookie("__utmz") != null){
			k_config.consoleLog('Getting "utmz" timestamp cookie');
			k_config.getGAcookies();
		}
		if(typeof(k_config.params.utmz_timestamp)!='undefined'){
			k_config.consoleLog('Using "utmz" timestamp cookie');
			// Unix time
			var st = k_config.params.utmz_timestamp*1000;
		} else if (k_config.Get_Cookie("session_start_time") !== null) {
			k_config.consoleLog('Using Kampyle "session_start_time" timestamp cookie');
			// Epoch time (1000 x Unix time)
			var st = k_config.Get_Cookie("session_start_time");  
		} else {
			k_config.consoleLog('Neither timestamp cookie found');
			return false;
		}
		// Same function for both cookies
		var timeNow = (new Date()).getTime();
		var secsElapsed = Math.round((timeNow - st) / 1000);
		var hr = Math.floor(secsElapsed/3600);
		var min = Math.floor((secsElapsed-(hr*3600))/60);
		var sec = Math.floor(secsElapsed-(hr*3600)-(min*60));
		hr = (hr<10) ? "0"+hr : hr;
		min = (min<10) ? "0"+min : min;
		sec = (sec<10) ? "0"+sec : sec;
		k_config.params.time_OnSite = hr + ':' + min + ':' + sec;			
		k_config.params.time_OnSiteInSecs = secsElapsed;
		k_config.params.time_OnSiteInMins = Math.floor((secsElapsed)/60);	
		
	},

	sessionCounter: function(){
		if(typeof(k_config.params.utmz_numberOfSessions)=='undefined' && k_config.Get_Cookie("__utmz") != null){
			k_config.consoleLog('Getting "utmz" session count cookie');
			k_config.getGAcookies();
		}
		if(typeof(k_config.params.utmz_numberOfSessions)!='undefined'){
			k_config.consoleLog('Using "utmz" session count cookie');
			// Unix time
			var sc = k_config.params.utmz_numberOfSessions;
		} else if (k_config.Get_Cookie("k_visit") !== null) {
			k_config.consoleLog('Using Kampyle "k_visit" session count cookie');
			var sc = k_config.Get_Cookie("k_visit");  
		} else {
			k_config.consoleLog('Neither session count cookie found');
			return false;
		}
		// Same for both cookies
		k_config.params.sessionCounter = sc;		
	},

	getPlugins: function(src){

		var k = [], i, p, m, t, keys, console, plugins;
		var n = navigator.plugins;
		var nl = n.length;
		for (i = 0; i < nl; i++) {
			p = n[i];
			k[i] = p.name;
			k_config.consoleLog(i + ' ' + p.name,8);
			if (p.description != '' && p.description != p.name) {
				//* Replace duplication to minimize URL length (844 > 746) 
				p.shortDesc = p.description.replace(p.name, '$');
				//* Keep description short to minimize URL length (746 > 691) 
				var maxDesc = p.name.length;
				if (p.shortDesc.length > maxDesc) {
					//*Retain any numbers we would otherwise cut 
					var re = new RegExp("[^A-Za-z$\s]", "g");
					var txt = new RegExp("[0-9\s]", "g");
					var result = p.shortDesc.substr((maxDesc) - 3).split(" ");
					var r = "";
					for (keys in result) {
						try {
							k_config.consoleLog(i + ' ' + keys + ' looking for numbers... ' + result[keys],8);
							t = result[keys].match(txt);
							if (t != null) {
								m = result[keys].match(re);
								if (m) {
									k_config.consoleLog(i + ' match!  ' + m,8);
									r += result[keys] + ' ';
								}
							}
						} catch(e) {
							k_config.consoleLog('plugin_error ('+e+')',8);
							k_config.params.k_error += '|plugin_error('+e+')';
						}
					}
					// r is the numbers we would otherwise have lost... 
					r = r.replace(/^\s\s*\--added this to avoid closing comment tag   /, '').replace(/\s\s*$/, '');
					var add = '(' + p.shortDesc.substr(0, (maxDesc) - 3) + '...' + r + ')';
					k[i] += add;
					k_config.consoleLog(i + ' add ' + add,8);
				} else {
					k_config.consoleLog(i + ' = Short enough! Adding original',8);
					k[i] += '(' + p.shortDesc + ')';
				}
			} else {
				k_config.consoleLog(i + ' No description '+p.description,8);
			}
		}
		
		k_config.params.plugins = k.toString();
		k_config.consoleLog(k_config.params.plugins,8);
	},

	getProductName: function(elem,tag,count){
		if (!count) {var count = 0}
		try{
			if(elem && tag){
				var t = elem.getElementsByTagName(tag);
				var l = t[count].innerHTML.replace(/\&amp;/g,'&');
				return l;
			}
			
			if(elem){
				var l = elem.innerHTML.replace(/\&amp;/g,'&');
				return l;
			}
		
			else{
				return 'Not Found';
			}
		}catch(err){
			k_config.consoleLog('getProductName error (Code: '+err+')',1);
			k_config.params.k_error += '|getProductName '+err;
		}
	},

	
	// USER EXPERIENCE SCRIPTS
	
	lightboxLoad: function(){
		// Load Lightbox CSS
		try{
			if(typeof (k_config.site.url_lightbox_css) != 'undefined'){
			k_config.consoleLog('LIGHTBOX - Loading CSS: '+ k_config.site.protocolLoad+k_config.site.url_lightbox_css);
				k_config.loadjscssfile(k_config.site.protocolLoad+k_config.site.url_lightbox_css, "css");
			}
		}catch(err){
			k_config.consoleLog('LIGHTBOX - CSS Load Error ('+err+')');
		}
		// Load Lightbox JS
		try{
			k_config.consoleLog('LIGHTBOX - Lightbox JS: '+ k_config.site.protocolLoad+k_config.site.url_lightbox_js);
			k_config.loadScript(k_config.site.protocolLoad+k_config.site.url_lightbox_js,"jQuery.ui",k_config.lightboxReady);
		}catch(err){
			k_config.consoleLog('LIGHTBOX - JS Load Error ('+err+')');
		}
	},
	
	lightboxReady: function(){
		
		k_config.consoleLog('LIGHTBOX - Assigning click event to #kampylink');
	
		$(document).ready(function() {
			var $dialog = $('<div></div>')
				.html('This dialog will show every time!')
				.dialog({
					autoOpen: false,
					title: 'Basic Dialog'
				});

			$('#'+k_config.site.buttonID).click(function() {
				$dialog.dialog('open');
				// prevent the default action, e.g., following a link
				return false;
			});
		});
	},

	colorboxInit: function() {
		k_config.consoleLog('COLORBOX - Binding on ready');
		//alert(k_config.site.buttonID);
		$(document).ready(function() {	
				$('#'+k_config.site.buttonID).live('click', k_config.colorboxBindToButton);
		});
	},
	
	colorboxBindToButton: function () {	
		var closeHTML = "<img src='http://www.nytimes.com/images/article/comments/buttons/close_window.gif'>";
		document.getElementById('cboxLoadingGraphic').innerHTML="Loading Kampyle Feedback Form for NYTimes...";
		$('#'+k_config.site.buttonID).die('click', k_config.colorboxBindToButton);
		$.fn.colorbox({iframe:true, href:$(this).attr('href'), width:500, height:500, fastIframe:false, close:closeHTML}); 
		$('#'+k_config.site.buttonID).colorbox({iframe:true, href:$('#'+k_config.site.buttonID).attr('href'), width:500, height:500, fastIframe:false, close:closeHTML});
		return false; 
	}

}
// === Kampyle Core Code - Do Not Edit Above This Line =======================================================================================
// === Kampyle Customer Code - Only Edit Below This Line =====================================================================================

// List of client cookies - all cookies in this list will be parsed if a specific cookie name is not requested
k_config.cookieList = ["_mkto_trk","_wsad_","citrix-edoc-search"];

/* Defaults Only - will be set by siteSection */
k_config.site = {
	"site_name":"CitrixSupport",
	"site_shortName":"CTRX",
	"site_code": "3961704",
	"button_form_id": "20899",
	"inviteOnly": false,
	"buttonClass": "k_float k_bottom k_right",
//	"buttonClass": "callbacks cboxElement",
//	"useHref":false,
//	"buttonID":"kampylink",
//	"formURL": "",
	"lang": "en",
	"serverRegion":"www.kampyle.com",
	"lightbox":false,

	"testMode":false,
	"showButton":true,	
	"addProtocol": true,
	"url_css": "www.citrix.com/static/kampyle/prod/css/k_button.css",
	"url_kbutton": "www.citrix.com/static/kampyle/prod/js/k_button.js",
	"url_kpush": "www.citrix.com/static/kampyle/prod/js/k_push.js",
	"url_buttonImage": "www.citrix.com/static/kampyle/prod/images/icon-feedback-on.png",
	"url_inviteFolder": "cf.kampyle.com/",
	"url_jQuery": "",
	"url_lightbox_js":"lightbox/jquery+jquery-ui.min.combined.js",
	"url_lightbox_css":"lightbox/jquery-ui.css",


	"forceProtocol":"",
	"forceLoad_css":"",
	"forceLoad_push":"",
	"forceLoad_button":"",	
	"revision":"v5.5_11Sept12"
}

// Backward compatibility for localHost - this will be deprecated in the future

if (k_config.site.localHost == 'true' || k_config.site.localHost == true){
	k_config.site.addProtocol = false;
	k_config.consoleLog('localHost is depreciated',99);
	k_config.params.k_error += '|localHost is depreciated';
}
if (k_config.site.localHost == 'false' || k_config.site.localHost == false){
	k_config.site.addProtocol = true;
	k_config.consoleLog('localHost is depreciated',99);
	k_config.params.k_error += '|localHost is depreciated';
}

// Set protocols to be used for loading 

k_config.site.protocolForm = ("https:" == window.location.protocol) ? "https://" : "http://";
k_config.site.protocolLoad = ("https:" == window.location.protocol) ? "https://" : "http://";

if (k_config.site.addProtocol == false || k_config.site.addProtocol == "false"){ 
                k_config.site.protocolLoad = "";                
} else if (k_config.site.forceProtocol){ 
                k_config.site.protocolLoad = k_config.site.forceProtocol;
                k_config.site.protocolForm = k_config.site.forceProtocol;
} else {
                k_config.site.protocolLoad = ("https:" == window.location.protocol) ? "https://" : "http://";
                k_config.site.protocolForm = ("https:" == window.location.protocol) ? "https://" : "http://";
}

/* Defaults Only - will be set by siteSection */
k_push_vars = { 
	"display_after_on_page": 10,
	"display_after": 10,  			
	"view_percentage": 0,
	"popup_font_color": "#000000",
	"popup_background": "#ffffff",
	"popup_separator": "#D4E2F0",
	"popup_manual": "true",
	"header": "Your feedback is important to us!",
	"question": "Would you be willing to give us a short (1 minute) feedback?",
	"footer": "Thank you for helping us improve our website",
	"remind": "Remind me later",
	"remind_font_color": "#3882C3",	
	"yes": "Yes",
	"no": "No",
	"text_direction": "ltr",
	"images_dir": k_config.site.protocolLoad+k_config.site.url_inviteFolder,
	"yes_background": "#76AC78",
	"no_background": "#8D9B86",
	"site_code": k_config.site.site_code,
	"ff_link_id": "k_popup_link",
	"invite_form_id": "85502"
};

k_config.friendlyNames = {
	"k_error": "errorLog",
	"navLevel0": "domain",
	"refDiffHost": "LandingPage"
};

// testmode=true in URL will over-ride explicit setting in site.  Default: testmode=false
if (typeof (k_config.site.testMode) == "undefined" || k_config.site.testMode == ""){ k_config.site.testMode=false; }
db_url = /testmode=([0-9]|true|false|lite|light)\b/i;
db_test = window.location.href.match(db_url);
if (db_test != null){
	switch(db_test[1])
	{
	case '0':
	case 'false':
		k_config.site.testMode = false;
		k_config.consoleLog('testMode set from URL: '+k_config.site.testMode);
		break;
	case '1':
	case 'true':
		k_config.site.testMode = 1;
		k_config.consoleLog('testMode set from URL: '+k_config.site.testMode,1);
		break;
	default:
		try{
			k_config.site.testMode = db_test[1];
			k_config.consoleLog('testMode set from URL: '+k_config.site.testMode,db_test[1]);
			break;
		} catch (err) {
			k_config.consoleLog('testMode unsuccessfully detected in URL: '+db_test+' ('+ err +')');
		}
	}
}else{
	k_config.consoleLog('testMode not detected in URL: '+db_test);
}



// CUSTOM FUNCTIONS =========================================================================================


k_config.getMeta = function(attrValue,valToReturn,attrName) {
		if((attrName==""||attrName==undefined)&&(valToReturn==""||valToReturn==undefined)){
			var n = "m_";
			var metaList = document.getElementsByTagName('meta');
			var mLen = metaList.length;
			for (var i = 0; i < mLen; i++){
				if (metaList[i].name==""||metaList[i].name==undefined) { continue; }
				k_config.params[n + metaList[i].name] = metaList[i].content;
				k_config.consoleLog('getMeta adding: ['+n + metaList[i].name+' = '+metaList[i].content+']');			
			}
		}
		if(attrName==""||attrName==undefined){attrName="name"}
		if(valToReturn==""||valToReturn==undefined){valToReturn="content"}
		var m = document.getElementsByTagName('META');
		var i;
		for (i = 0; i < m.length; i++){
			if (m[i].getAttribute(attrName) == attrValue){
				return m[i].getAttribute(valToReturn);
			}
		}
	  return null;
}


k_config.getBreadcrumbs_eDocs = function(elem){

	/*k_config.crumbArray = [];

			k_config.consoleLog('Custom getBreadcrumbs()');
			
			k_config.params.navFull = "noBreadcrumbs";

			var dirs = document.getElementsByClassName("dir");
			var dlen = dirs.length;

			for (var i=0; i<dlen; i++) {
				var crumb = dirs[i].getElementsByTagName('A')[0].text;
				k_config.crumbArray[i+1] = crumb;
				k_config.consoleLog('getBreadcrumbs: ' + crumb);
			}
			
			k_config.consoleLog('getBreadcrumbs: ' +JSON.stringify(k_config.crumbArray));
	*/

	var dirs, dlen, i, crumb, crumbArray;
		try{
			k_config.crumbArray = [];
			var elem = document.getElementById('gwt-debug-breadcrumbPanel');
			var dirs = elem.getElementsByTagName("li");		
			var dlen = dirs.length;

			for (i = 0; i < dlen; i +=1) {

				if(typeof(dirs[i].firstChild.innerText)!="undefined" && dirs[i].firstChild.innerText != ""){
				/* For IE and Chrome */
				k_config.consoleLog('innerText');
				crumb = dirs[i].firstChild.innerText;
				}else if(typeof(dirs[i].firstChild.innerHTML)!="undefined" && dirs[i].firstChild.innerHTML != ""){
				/* For FF */
				k_config.consoleLog('innerHTML');
				crumb = dirs[i].firstChild.innerHTML;
				}else if(typeof(dirs[i].firstChild.text)!="undefined" && dirs[i].firstChild.text != ""){
				/* Fallback */
				k_config.consoleLog('text');
				crumb = dirs[i].firstChild.text;
				}
				k_config.crumbArray[i+1] = crumb;
				k_config.consoleLog('getBreadcrumbs: ' + crumb);
			}
		}catch(e){
			k_config.consoleLog('Breadcrumbs Error'+e);
			return;
		}

		try{
			k_config.consoleLog('getBreadcrumbs: ' +JSON.stringify(k_config.crumbArray));
		}catch(e){
			k_config.consoleLog('getBreadcrumbs: JSON Error');
		}



		
		if ((typeof(k_config.crumbArray[1])!="undefined")){

						// Only change the following code you want to customize the syntax of the k_config.params.navFull string
						k_config.params.navFull = ""
						+((typeof(k_config.crumbArray[0])!="undefined")&&(k_config.crumbArray[0]!="")?k_config.crumbArray[0]:"")
			+((typeof(k_config.crumbArray[0])!="undefined")&&(typeof(k_config.crumbArray[1])!="undefined")&&( k_config.crumbArray[0]!="")?":":"")
						+((typeof(k_config.crumbArray[1])!="undefined")? k_config.crumbArray[1]:"")
						+((typeof(k_config.crumbArray[2])!="undefined")?":"+ k_config.crumbArray[2]:"")
						+((typeof(k_config.crumbArray[3])!="undefined")?":"+ k_config.crumbArray[3]:"")
						+((typeof(k_config.crumbArray[4])!="undefined")?":"+ k_config.crumbArray[4]:"")
						+((typeof(k_config.crumbArray[5])!="undefined")?":"+ k_config.crumbArray[5]:"")
						+((typeof(k_config.crumbArray[6])!="undefined")?":"+ k_config.crumbArray[6]:"")
						
			k_config.consoleLog('getBreadcrumbs: ' +k_config.params.navFull);	
		}

		// Set NavLevels based on breadcrumbs – starting at NavLevel1 as NavLevel0 is reserved for domain
		if ((typeof(k_config.crumbArray[1])!="undefined")){
						k_config.params.navLevel1 = (typeof(k_config.crumbArray[1])!="undefined")?k_config.crumbArray[1]:"";
						k_config.params.navLevel2 = (typeof(k_config.crumbArray[2])!="undefined")?k_config.crumbArray[2]:"";
						k_config.params.navLevel3 = (typeof(k_config.crumbArray[3])!="undefined")?k_config.crumbArray[3]:"";
						k_config.params.navLevel4 = (typeof(k_config.crumbArray[4])!="undefined")?k_config.crumbArray[4]:"";
						k_config.params.navLevel5 = (typeof(k_config.crumbArray[5])!="undefined")?k_config.crumbArray[5]:"";
		}
}




k_config.getParamsOnRun = function(src){

	// This runs just once when page is loaded - to re-evaluate for dynamic pages etc use: k_config.getParams() instead
	k_config.consoleLog('getParamsOnRun('+src+')');
	
	// Get filename
	k_config.params.fileName = k_config.getFilename(document.location.href);
	
	// Get URL path
	var wlp = window.location.pathname;
	k_config.setPathLevels(wlp);
	
	// Get All Query Strings
	k_config.params.pageQStr = window.location.search;
	k_config.setAllQStr(k_config.params.pageQStr,"","url_");
	
	// Get Search Engine details


	k_config.searchEngine();
	k_config.getGAcookies();
	k_config.sessionCounter();

	// Get Site Cookies
	k_config.getCookieList("cookie_");
	
	// Get All Metadata
	k_config.getMeta();
	
	// Update breadcrumbs
	k_config.getBreadcrumbs_eDocs();

	//Check for onpage override - by default will check for k_pageParams
	k_config.loadSectionConfig('','k_pageParams');
	
}

k_config.getParams = function(src){
var gp_start = new Date();

	// Custom handler function.  Overwritting core code.
	k_config.consoleLog('Custom getParams('+src+')');
	
	// Refresh URL parameters for sites with dynamic content
	k_config.params.hostName = window.location.hostname;
	k_config.params.pagePath = window.location.pathname;
	k_config.params.pageQStr = window.location.search;
	k_config.params.pageHash = window.location.hash.substr(1);
	k_config.params.pageTitle = document.title;
	
	// Get filename
	k_config.params.fileName = k_config.getFilename(document.location.href);

	// Get URL path
	var wlp = window.location.pathname;
	k_config.setPathLevels(wlp);

	// Get All Query Strings
	k_config.setAllQStr(k_config.params.pageQStr,"","url_");

	// Get referrer details
	var dRef = document.referrer;
	k_config.params.refHostName = (dRef) ? dRef.match(/:\/\/(.[^/]+)/)[1] : "direct";
	k_config.params.refURL = (dRef) ? unescape(dRef) : "direct";
	k_config.params.refFilename = (dRef) ? k_config.getFilename(dRef) : "direct";
	k_config.params.refDiffHost = (dRef) ? ((k_config.params.refHostName==k_config.params.hostName)?"SameHost":"DiffHost") :"direct";
	
	// Get Site Cookies
	k_config.getCookieList("cookie_");
	
	// Get Viewport
	var w=window;
	var d=document;
	var e=d.documentElement;
	var g=d.getElementsByTagName('body')[0];
	var x=w.innerWidth||e.clientWidth||g.clientWidth;
	var y=w.innerHeight||e.clientHeight||g.clientHeight;
	k_config.params.windowWidth = x;
	k_config.params.windowHeight = y;
	k_config.params.windowSize = x+'x'+y;
	
	// Get specified parameter (either array, parameter or object)
	k_config.getOnPageParams();	
	
	//Check for onpage override - by default will check for k_pageParams
	k_config.loadSectionConfig('','k_pageParams');
	
	// Get timeOnSite details from Google or Kampyle cookie
	k_config.getTimeOnSite();
	
	
	// Move all k_config.params to k_button.extraParams (if possible) before end of each k_getParams
	if (typeof (k_button)!="undefined"){
		k_config.setCustomVariables();
	}
	
	// Update attributes for the button object
	try{
		if(typeof (k_button) != "undefined"){
			var k_link = document.getElementById(k_config.site.buttonID);
			var k_linkURL = k_config.site.protocolForm + k_config.site.serverRegion + k_config.site.formURL + k_config.getQStr(k_config.buttonParams());			
			k_config.consoleLog('Update href: '+k_linkURL,2);
			k_link.setAttribute('href',k_linkURL);
			k_link.setAttribute('target',k_config.site.buttonTarget);
			
		}else{
			k_config.consoleLog('no_k_buttonForHref('+src+')',2);
			k_config.params.k_error += '|no_k_buttonForHref('+src+')';
		}
	}catch(err){
		k_config.consoleLog('href_error('+src+') (Code: '+err+')',2);
		k_config.params.k_error += '|href_error('+src+') '+err;
	}
	

var gp_done = new Date() - gp_start;
k_config.consoleLog('TIMER: getParams('+src+') took: ' + gp_done + 'ms',2);
}





k_config.siteSections = { 



	// Support Areas
//-----------------------------------------------------
	"KnowledgeCenter": {
		"host":"support",
		"pathLevel1":"",
		"unit":"support",
		"siteSection":"KnowledgeCenter",
		"re":"",
		"reMods":"i",
		"stopOnThisMatch":false,
		"invite" : { "invite_form_id": "85482", "display_after_on_page": 30, "display_after": 60, "view_percentage": 0 },
		"button" : { "button_form_id": "89982", "showButton": true }
	},
		
//-----------------------------------------------------	
	"eDocs": {
		"pathLevel1":"proddocs",
		"unit":"support",
		"siteSection":"eDocs",
		"re":"",
		"reMods":"i",
		"stopOnThisMatch":false,
		"invite" : { "invite_form_id": "85492", "display_after_on_page": 30, "display_after": 60, "view_percentage": 0 },
		"button" : { "button_form_id": "84912", "showButton": true }
	},
//-----------------------------------------------------
	"CTV": {
		"pathLevel1":"tv",
		"unit":"support",
		"siteSection":"CTV",
		"re":"",
		"reMods":"i",
		"stopOnThisMatch":false,
		"invite" : { "invite_form_id": "85482", "display_after_on_page": 30, "display_after": 60, "view_percentage": 0 },
		"button" : { "button_form_id": "84902", "showButton": true }
	},
//-----------------------------------------------------
	"Events": {
		"host":"(event|synergy)",
		"pathLevel1":"",
		"unit":"events",
		"siteSection":"Events",
		"re":"",
		"reMods":"i",
		"stopOnThisMatch":false,
		"invite" : { "invite_form_id": "85482", "display_after_on_page": 30, "display_after": 60, "view_percentage": 0 },
		"button" : { "button_form_id": "89922", "showButton": true }
	},
//-----------------------------------------------------
	"Blogs": {
		"host":"blog",
		"pathLevel1":"",
		"unit":"blogs",
		"siteSection":"Blogs",
		"re":"",
		"reMods":"i",
		"stopOnThisMatch":false,
		"invite" : { "invite_form_id": "85482", "display_after_on_page": 30, "display_after": 60, "view_percentage": 0 },
		"button" : { "button_form_id": "89932", "showButton": true }
	},
//-----------------------------------------------------
//-----------------------------------------------------
	"Forum": {
		"host":"forums|discussions|ipboard",
		"pathLevel1":"",
		"unit":"forum",
		"siteSection":"Forums",
		"re":"",
		"reMods":"i",
		"stopOnThisMatch":false,
		"invite" : { "invite_form_id": "85482", "display_after_on_page": 30, "display_after": 60, "view_percentage": 0 },
		"button" : { "button_form_id": "89952", "showButton": true }
	},
//-----------------------------------------------------
	"Education": {
		"host":"training",
		"pathLevel1":"",
		"unit":"education",
		"siteSection":"Education",
		"re":"",
		"reMods":"i",
		"stopOnThisMatch":false,
		"invite" : { "invite_form_id": "85482", "display_after_on_page": 30, "display_after": 60, "view_percentage": 0 },
		"button" : { "button_form_id": "89962", "showButton": true }
	},
//-----------------------------------------------------
	"Demo": {
		"host":"demo",
		"pathLevel1":"",
		"unit":"demo",
		"siteSection":"Demo",
		"re":"",
		"reMods":"i",
		"stopOnThisMatch":false,
		"invite" : { "invite_form_id": "85482", "display_after_on_page": 30, "display_after": 60, "view_percentage": 0 },
		"button" : { "button_form_id": "89972", "showButton": true }
	},
//-----------------------------------------------------
	"Project": {
		"host":"project",
		"pathLevel1":"",
		"unit":"project",
		"siteSection":"PojectAccelerator",
		"re":"",
		"reMods":"i",
		"stopOnThisMatch":false,
		"invite" : { "invite_form_id": "94881", "display_after_on_page": 30, "display_after": 60, "view_percentage": 0 },
		"button" : { "button_form_id": "94881", "showButton": true }
	},	
	
	
//-----------------------------------------------------

	// Domains

	"dev_environments": {
		"host":".*?(qa|dev)",
		"unit":"DEV_ENVIRONMENT",
		"re":"",
		"reMods":"i",
		"stopOnThisMatch":false
	},


	
	
//--------------------------------------

	// Test - force display	
	"test": {
		"navLevel1":"",
		"navLevel2":"",
		"navLevel3":"",
		"navLevel4":"",
		"navLevel5":"",
		"navFull":"",
		"sectionType":"TEST_SECTIONTYPE",
		"content":"TEST_CONTENT",
		"product":"TEST_PRODUCT",
		"siteSection":"TEST_SECTION",
		"re":"showbutton=true",
		"reMods":"i",
		"stopOnThisMatch":true,
		"invite" : { "invite_form_id": k_push_vars.invite_form_id, "display_after_on_page": 3, "display_after": 6, "view_percentage": 100 },
		"button" : { "button_form_id": k_config.site.button_form_id, "showButton": true, "inviteOnly": false, "url_buttonImage": "", "buttonClass": "k_float k_bottom k_right" }
	}
}

// Key Parameters
// MOVED UP

// Evaluate current page before sending to evalPage for evaluation
k_config.getParamsOnRun('BeforeEvalPage');

// Evaluate current page and assign siteSection and push_vars
k_config.evalPage(window.location.host.toLowerCase(),'host');
k_config.evalPage(window.location.href.toLowerCase(),'pathLevel1');


if(typeof(k_config.params.url_locale)!="undefined"){
	k_config.site.lang = k_config.params.url_locale;
}

//Check for onpage override - by default will check for k_pageParams
k_config.loadSectionConfig('','k_pageParams');

// Required if using conditional buttons
if (typeof (k_button)!="undefined"){
  k_config.k_button_done = 1;
}
// Finally - check all is good and add tags in order to display button
if(k_config.site.showButton=="true"||k_config.site.showButton==true){

	if(k_config.site.lightbox=="true"||k_config.site.lightbox==true){
		// Works with customPipeMessage() in Additional Sentance to close Colorbox after user click on <Cancel> or <Send>
		var XD=function(){var a,b,c=1,d,e=this;return{postMessage:function(a,b,d){if(!b){return}d=d||parent;if(e["postMessage"]){d["postMessage"](a,b.replace(/([^:]+:\/\/[^\/]+).*/,"$1"))}else if(b){d.location=b.replace(/#.*$/,"")+"#"+ +(new Date)+c++ +"&"+a}},receiveMessage:function(c,f){if(e["postMessage"]){if(c){d=function(a){if(typeof f==="string"&&a.origin!==f||Object.prototype.toString.call(f)==="[object Function]"&&f(a.origin)===!1){return!1}c(a)}}if(e["addEventListener"]){e[c?"addEventListener":"removeEventListener"]("message",d,!1)}else{e[c?"attachEvent":"detachEvent"]("onmessage",d)}}else{a&&clearInterval(a);a=null;if(c){a=setInterval(function(){var a=document.location.hash,d=/^#?\d+&/;if(a!==b&&d.test(a)){b=a;c({data:a.replace(d,"")})}},100)}}}}}()
		XD.receiveMessage(function(message){
		if(message.data == 'closeLightbox')
			$.colorbox.close();
		}, 'http://www.kampyle.com');
		// Now Initialize...
		k_config.colorboxInit();
	}else{
		k_config.consoleLog('Lightbox - Not Enabled');
	}
	
	// Bind New Feedback Button <div> to DOM
	k_config.bindReady(function() {
		k_config.createKampyleTags();
	});
}else{
	k_config.consoleLog('TERMINATED. showButton='+k_config.site.showButton+' so createKampyleTags() was not executed!');
}

var k_time = new Date() - k_start;
k_config.params.k_loadtimeMS = k_time;
k_config.consoleLog('TIMER: Total Runtime: '+k_time+'ms',2);
