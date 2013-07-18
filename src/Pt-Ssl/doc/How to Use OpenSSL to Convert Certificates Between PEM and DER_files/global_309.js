jQuery(document).ready(function() {
	
		jQuery("#tabHotfix").click(function(){
			jQuery("#tab-topic2").css("display","none");
			jQuery("#tab-hotfix2").css("display","block");
			jQuery("#tab-doc2").css("display","none");
			jQuery("#tab-comp2").css("display","none");
		});
		jQuery("#tabTopic").click(function(){
			jQuery("#tab-topic2").css("display","block");
			jQuery("#tab-hotfix2").css("display","none");
			jQuery("#tab-doc2").css("display","none");
			jQuery("#tab-comp2").css("display","none");
		});
		jQuery("#tabDoc").click(function(){
			jQuery("#tab-topic2").css("display","none");
			jQuery("#tab-hotfix2").css("display","none");
			jQuery("#tab-doc2").css("display","block");
			jQuery("#tab-comp2").css("display","none");
		});
		jQuery("#tabComp").click(function(){
			jQuery("#tab-topic2").css("display","none");
			jQuery("#tab-hotfix2").css("display","none");
			jQuery("#tab-doc2").css("display","none");
			jQuery("#tab-comp2").css("display","block");
		});
		
	
		/*this portion handles the behavior of the alert box for the homepage */
		jQuery("#alertlnkclose").click(function(){
			jQuery("div#msgbox").slideUp("slow");	
		});
	
		//deflection survey
	if (jQuery("#deflectionForm").attr("name")==="deflectionFormDiv")
	{
		jQuery("#feedbackLink1").attr("href","#feedback");
		jQuery("#feedbackLink2").attr("href","#feedback");
		
		var ctxNumber = escape(jQuery("#ctxNumber").val());
		var baseUrl = "/kc/deflection";
		
		jQuery("#changeButton").click(function(){
				jQuery("#question1 > input").removeAttr("disabled");
				jQuery("#question1").removeClass("disabledText");
				jQuery("#yesPath").hide();
				jQuery("#noPath").hide();
				jQuery("#answers2").hide();
				jQuery("#changeButton").hide();
				jQuery("#changeButton1").hide();
				jQuery("#thanks2").hide();
		});
		
		showYesPath = function()
		{
			jQuery("#yesPath").show();
			jQuery("#noPath").hide();
		}
		
		showNoPath = function()
		{
			jQuery("#noPath").show();
			jQuery("#yesPath").hide();
		}		
		
		prepareForGoogleAnalytics = function()
		{
			var question = jQuery("#question1 input:checked").val();
			var answer = jQuery("#answers2 input:checked").val();
		
			var q1answer = jQuery("#question1 input:checked").val();
			var label0 = jQuery("#question0Label").html();
			var label1 = jQuery("#question1Label").html();
			var label2 = jQuery("#question2Label").html();

			if (q1answer === "didSolve")
			{
				question = "Deflection Survey Q1: " + label0;
			}
			else if (q1answer === "didNotSolve")
			{
				question = "Deflection Survey Q1: " + label0;
			}
			else if (q1answer === "needTest")
			{
				question = "Deflection Survey Q1: " + label0;
			}
			else if (q1answer === "notSure")
			{
				question = "Deflection Survey Q1: " + label0;
			}
			else if (q1answer === "browsing")
			{
				question = "Deflection Survey Q1: " + label0;
			}
			//alert("Q == " + question + " A == " + q1answer);

			sendToGoogleAnalytics(question, q1answer);
		}

		jQuery("#didSolve").click(function(){
			answer1();
			showYesPath()
			//jQuery("#yesPath").removeAttr("disabled");
			jQuery("#answers2").show();
			//jQuery("#answers2").removeAttr("disabled");
			disableQuestion1(true);
			
			prepareForGoogleAnalytics();
		});

		jQuery("#answers2").click(function(){
			var question = jQuery("#question1 input:checked").val();
			var answer = jQuery("#answers2 input:checked").val();
		
			var q1answer = jQuery("#question1 input:checked").val();
			var label0 = jQuery("#question0Label").html();
			var label1 = jQuery("#question1Label").html();
			var label2 = jQuery("#question2Label").html();

			if (q1answer === "didSolve")
			{
				question = "Deflection Survey Q2Y: " + label1;
			}
			else if (q1answer === "didNotSolve")
			{
				question = "Deflection Survey Q2N: " + label2;
			}
			//alert(question);

			sendToGoogleAnalytics(question, answer);
		});

		
		disableQuestion1 = function(clear2)
		{
			jQuery("#question1 > input").attr("disabled","disabled");
			jQuery("#question1").addClass("disabledText");
			jQuery("#changeButton").show();
			
			var answer2Radio = jQuery("#answers2 input:checked");
			if (clear2)
			{
				if (answer2Radio)
				{
					answer2Radio.removeAttr("checked");
				}
			}
			enableQuestion2();
		};
		
		disableQuestion2 = function()
		{
			jQuery("#question2Id > input").attr("disabled","disabled");
			jQuery("#question2Div").addClass("disabledText");
			jQuery("#changeButton1").show();
			jQuery("#thanks2").show();
		};

		enableQuestion2 = function()
		{
			jQuery("#question2Id > input").removeAttr("disabled");
			jQuery("#question2Div").removeClass("disabledText");
			jQuery("#changeButton1").hide();
			jQuery("#thanks2").hide();
		};	
		
		didNotSolveF = function(){
			answer1();
			showNoPath()
			jQuery("#answers2").show();
			disableQuestion1(true);
			
			prepareForGoogleAnalytics();
		};
		
		noFurtherAction = function()
		{
			answer1();
			disableQuestion1(true);
			jQuery("#thanks2").show();
			
			prepareForGoogleAnalytics();
		};

		answer1 = function(){
			var answer1value = jQuery("#deflectionForm input:checked").val();
			var url= baseUrl + "?ctxNumber="+ctxNumber+"&answer1="+answer1value;
			$.post(url);
		};
		
		answer2 = function(){
			disableQuestion2();
			var answer2value = jQuery("#answers2 input:checked").val();
			var url= baseUrl + "?ctxNumber="+ctxNumber+"&answer2="+answer2value;
			$.post(url);
			jQuery("#changeButton1").show();
		};
		
		enableBugComments = function()
		{
			jQuery("#additionalcommentsLink").hide();
			jQuery("#feedbackTextArea").attr("value","");
			jQuery("#feedbackTextArea").removeAttr("disabled");
			jQuery("#additionalcomments").show();
			jQuery("#feedbackTextArea").focus();
			return false;
		};
		
		
		
		jQuery("#didNotSolve").click(didNotSolveF);
		jQuery("#needTest").click(noFurtherAction);
		jQuery("#notSure").click(didNotSolveF);		
		jQuery("#browsing").click(noFurtherAction);
		
		jQuery("#supportcase").click(answer2);
		jQuery("#advisor").click(answer2);
		jQuery("#searchmore").click(answer2);
		jQuery("#noncitrix").click(answer2);
		jQuery("#ignore").click(answer2);		
		jQuery("#changeButton1").click(enableQuestion2);		
		
		jQuery("#submitFeedback").click(function(){
			var feedback = escape(jQuery("#feedbackTextArea").val());
			if (feedback=="")
			{
				alert("Cannot submit empty comments.");
				return;
			}else if (feedback.length>3000){
				alert("Please limit your comments to 3000 characters");
				return;
			}
			jQuery("#thanks").show();
			jQuery("#submitFeedback").hide();
			jQuery("#feedbackTextArea").attr("disabled","disabled");
			jQuery("additionalcomments").addClass("disabledText");
			var feedback = escape(jQuery("#feedbackTextArea").val());
			
			var url= baseUrl + "?ctxNumber="+ctxNumber+"&feedback="+feedback;
			jQuery.post(url);
		});
		
		jQuery("#feedbackLink1").click(enableBugComments);
		
		jQuery("#feedbackLink2").click(enableBugComments);
		
		if (focusfeedback)
		{
			jQuery("#feedbackTextArea").focus();
		}
		
		jQuery("#feedbackTextArea").keyup(function()
		{
			var feedback = escape(jQuery("#feedbackTextArea").val());
			
			if (feedback.length>3000){
				alert("Please limit your comments to 3000 characters");
				jQuery("#feedbackTextArea").attr("value",feedback.substring(0,3000));
			}
		});
		
		jQuery("#additionalcommentsLink").click(enableBugComments);
		
		if (runDisableQuestion1)
		{
			disableQuestion1();
		}

		if (runDisableQuestion2)
		{
			disableQuestion2();
		}		
	}
	
	if (hasAnchorFix) {
		/*=======================================================================
		inline_anchor_fixer.js
		Fixes issues with anchors in the our layout
		=======================================================================*/
	
		/* LOOKS FOR #P IN A URL */
		var query = self.document.location.hash;
						

		
		/* USED FOR PAGE ANCHORS */
		
		jQuery("a").click(function(){
			
			var url = jQuery(this).attr("href");
			
			
			/* CHECK TO SEE IF A LINK IS AN ANCHOR */
			if (url.search("#") != -1) {
				var posOfNumberSign = url.indexOf("#");
				var getAnchorName = url.slice(posOfNumberSign + 1);
				var anchorPos = 'name=' + getAnchorName;
				if (jQuery('[@' + anchorPos + ']').size() > 0)
				{
					var anchorPosTop = parseInt(jQuery('[@' + anchorPos + ']').offset().top);
				
				/* MOVE PAGE TO THE ANCHOR WHILE KEEPING OVERFLOW LAYER FROM MOVING */				
					jQuery("#ctxkcmain").scrollTop(0);
					jQuery(window).scrollTop(anchorPosTop);
					return false;
				} 
			}
		});
		
	}
	/*=======================================================================
	dynamic_links.js
	This does double-duty, handling the dynamic links and dynamic buttons.
	=======================================================================*/
	jQuery(".thickbox").each(function() {
		jQuery(this).attr("href", function() { 
			return jQuery(this).attr("jquerylogin");
		});
	});

	jQuery(".ctxtsbutton").each(function() {
		var btntype = jQuery(this).attr("type");
		var btnname = jQuery(this).attr("name");
		var btnclick = jQuery(this).attr("onclick");
		var linktype = "submit";
		var classes = "ctxtsbuttonlink";
		if (btntype == "reset") {
			linktype="reset";
			classes += " ctxtsresetlink";
		}
		
		var newLink = '<a href="#" class="'+classes+'" type="'+linktype+'" name="'+btnname+'" '+((btnclick!=null)?' onClick="'+btnclick+'"':'')+'><span>'+jQuery(this).attr("value")+'</span></a>';
		
		jQuery(this).after(newLink);
		jQuery(this).remove();
	});
	
	jQuery(".ctxtsbuttonlink").each(function() {
		var btnclick = jQuery(this).attr("onclick");
		if (btnclick == null) {
			var btntype = jQuery(this).attr("type");
			var btnname = jQuery(this).attr("name");
			var newHidden = '<input type="hidden" name="'+ btnname +'" value="true"/>';
			jQuery(this).bind("click", function(e){
				jQuery(this).before(newHidden);
				if(btntype=="reset") {

					jQuery(this).parents("form")[ 0 ].reset();
					jQuery("form")[0].reset()
					return false;

				}
				else {
					jQuery(this).parents("form")[ 0 ].submit();
				}
			});
		}
	});

	/* SCRIPT USED TO ALLOW FOR PEOPLE TO CLEAR FORM AND DO ADDITIONAL SEARCHES 
	   AFTER THE FIRST SEARCH THEY DO IN ADVANCED SEARCH */
	
	jQuery("a[@name=submitter]").click(function() {	
		jQuery("#searchform").submit();
	});
	
	jQuery(".ctxtsresetlink").click(function() {
		jQuery("#searchform").clearForm();
		/* jQuery("#searchterm").clearFields(); */
	});
	
	if (isGlobal) {

	/*=======================================================================
	navsearch.js
	=======================================================================*/
	
		jQuery("#searchtxt").value = "";
		jQuery("div#ctxkcnav dt a").click(function(){
			
			if (jQuery(this).parent().next().css("display") == "none") {
				jQuery("div#ctxkcnav dd:visible").hide("normal");
				jQuery(this).parent().siblings().removeClass("menuselected");		
				jQuery(this).parent().next().show("normal");
				jQuery(this).parent().removeClass("productlist").addClass("menuselected");
				jQuery(this).parent().addClass("productlist");
			}
			else {
				jQuery(this).parent().next().hide("normal");
				jQuery(this).parent().removeClass("menuselected").addClass("productlist");			
			}
			return false;
		});
			

		
		
	   	//Transfer qs to advanced search
	   	jQuery("#asadvancedsearchboxnavlink").click(function(){
			if (jQuery("#searchtxt").val()!="") {
				jQuery("#asadvancedsearchboxnavlink").attr("href",function(){
					return jQuery("#asadvancedsearchboxnavlink").attr("href") + "?qs=" + escape(jQuery("#searchtxt").val());
				});
			}
		});	
	
		/*=======================================================================
		from searchGSAmacro.ftl
		=======================================================================*/
			var result;
			
				jQuery("#searchtxt").focus(function() {
					document.forms['searchkc'].elements['searchtxt'].value = "";
				});
			
			
				jQuery("#gsasearchbtn").click(function() {
					gsaval(jQuery("#searchtxt"));
					return result;
				});	
	}
	
	if (isSiteMap) {
		/*=======================================================================
		was inline on sitemap.ftl
		=======================================================================*/
				jQuery("#sitemaplist").Treeview();
				jQuery("#asresourceitem").removeClass().addClass("lastCollapsable");
	}
	
	if (isErrorPage) {
		jQuery.post("/badurl.jsp",{ referrer: document.referrer, location: document.location });
	}
	
	if (hasLangSwitch) {
		jQuery(".langSubmit").css("display","none");
		jQuery(".langSelect").bind("change", function(){
	  		jQuery(this).parent().submit();
	  	});
		

  		
  	}
	
	if (hasSearchHelp) {
		var $pane = jQuery("#testdiv");
		var paneTop = parseInt($pane.offset().top);
		
		jQuery("dd:not(:first)").hide();

		jQuery("dt a").click(function(){
			jQuery("dd:visible").slideUp("normal");
			jQuery(this).parent().next().slideDown("normal");
			return false;
		});
		

		/* LOADING SIMPLE SEARCH PAGE AND INITIALIZING ANCHORS */	
		jQuery("#test").load("/kc/popupsearch/basic", function(){

			var $ssr = jQuery("#simpleSearchRefining");
			var ssrTop = parseInt($ssr.offset().top);
			var $ssk = jQuery("#simpleSearchKeeping");
			var sskTop = parseInt($ssk.offset().top);
			
			jQuery("#anchorRyr").click(function(){
				jQuery("#testdiv").scrollTop(ssrTop - paneTop);
			});
			jQuery("#anchorKtoyrp").click(function(){
				jQuery("#testdiv").scrollTop(sskTop - paneTop);
			});
		});


		/* RETURNS SIMPLE SEARCH PAGE BACK TO IT'S TOP */	
		jQuery("#bs").click(function(){
			jQuery("#test").load("/kc/popupsearch/basic", function(){
				jQuery("#testdiv").scrollTop(0);
			});
		});

		
		/* LOADING ADVANCED SEARCH PAGE AND INITIALIZING ANCHORS */
		jQuery("#as").click(function(){
			jQuery("#test").load("/kc/popupsearch/advanced", function(){
				jQuery("#testdiv").scrollTop(0);
				
				var $astc = jQuery("#advSearchTechContent");
				var astcTop = parseInt($astc.offset().top);
				var $asf = jQuery("#advSearchTechForum");
				var asfTop = parseInt($asf.offset().top);

				jQuery("#advSTechContent").click(function(){
					jQuery("#testdiv").scrollTop(astcTop - paneTop);
				});
				jQuery("#advSTechForums").click(function(){
					jQuery("#testdiv").scrollTop(asfTop - paneTop);
				});
			});
		});				

		/* LOADING SEARCH QUERIES TIP PAGE AND INITIALIZING ANCHORS */	
		jQuery("#sqt").click(function(){
			jQuery("#test").load("/kc/popupsearch/gsa", function(){
				jQuery("#testdiv").scrollTop(0);
				
				var $sqgsws = jQuery("#searchQuerygettingStartedWithSearch");
				var sqgswsTop = parseInt($sqgsws.offset().top);
				var $sqeys = jQuery("#searchQueryExpandingYourSearch");
				var sqeysTop = parseInt($sqeys.offset().top);
				var $sqrys = jQuery("#searchQueryRefiningYourSearch");
				var sqrysTop = parseInt($sqrys.offset().top);

				jQuery("#sqGettingStartedWithSpeach").click(function(){
					jQuery("#testdiv").scrollTop(sqgswsTop - paneTop);
				});
				jQuery("#sqExpandingYourSearch").click(function(){
					jQuery("#testdiv").scrollTop(sqeysTop - paneTop);
				});
				jQuery("#sqRefiningYourSearch").click(function(){
					jQuery("#testdiv").scrollTop(sqrysTop - paneTop);
				});
			});
		});	
	
	}
	
	/* ===================================================
	Google Analytics External Link/Download Sniffer
	=================================================== */				
	jQuery("a").click(function(){
		var pageUrl = document.location.href;
		var iLastSlash = pageUrl.lastIndexOf("/"); 
		var articleNumber = pageUrl.substr(iLastSlash); 
			
		var linkUrl = jQuery(this).attr("href");
		var iLastDot = linkUrl.lastIndexOf(".");
		var docType = linkUrl.substr(iLastDot); 

		/* CHECK TO SEE IF A LINK GOES TO AN EXTERNAL URL */
		if (linkUrl.search("http://") != -1) {
			var linkUrlName = linkUrl.slice(7)
			pageTracker._trackPageview('/outgoing/' + linkUrlName)
		}
	
		/* CHECK TO SEE IF A LINK GOES TO RSS */
		if (linkUrl.search("rss=on") != -1) {
			pageTracker._trackPageview('/rss/' + linkUrl)
		}
	});
	
	
	if (isSearch) {
		/*=======================================================================
		jquery_search.js
		=======================================================================*/
		searchInitialize("searchhelpsimple");
		transferQueryString();
	}
	if (isAdvancedSearch) {
		/*=======================================================================
		jquery_asearch.js
		=======================================================================*/
		searchInitialize("searchhelpadvanced");
		transferQueryString();

		jQuery("select#productversion").html("").attr("disabled","disabled");
		
	   	//Transfer qs back to basic search
	   	jQuery("#asbasicsearchlink").click(function(){
			if (jQuery("#searchterm").val()!="") {
				jQuery("#asbasicsearchlink").attr("href",function(){
					return jQuery("#asbasicsearchlink").attr("href") + "?qs=" + escape(jQuery("#searchterm").val());
				});
			}
		});
	
		jQuery(".aswarning").hide();
		
		jQuery("select#productline").html("").attr("disabled","disabled");
		jQuery("select#productversion").html("").attr("disabled","disabled");
		
			
		jQuery("input",".ascontenttypes").click(function(){
			if (jQuery("input:checked",".ascontenttypes").length == 0) {
				jQuery("#ascontenttypeswarning").css({ display: "inline" });
			}
			else {
				jQuery("#ascontenttypeswarning").css({ display: "none" });
			}
		});
		jQuery("input",".aslanguages").click(function(){
			if (jQuery("input:checked",".aslanguages").length == 0) {
				jQuery("#aslanguageswarning").css({ display: "inline" });
			}
			else {
				jQuery("#aslanguageswarning").css({ display: "none" });
			}
		});	
		
		
		jQuery.getJSON("/search/json", function(j){
	
			var f = j.familylist;
			
			jQuery("select#productline").removeAttr("disabled");
			jQuery("#productline").addOption("", "(All Products)", true);
			
			for (var i = 0; i < f.length; i++)
			{
				jQuery("#productline").addOption(f[i],f[i],false);
			}
			
			jQuery("#productline").selectOptions(currProduct).attr('selected', 'selected');
			if (jQuery("#productline option:selected").text()!="(All Products)") {
				versionPopulate(jQuery("#productline option:selected").val());
			}
			
		});
	
		jQuery("select#productline").change(function(){

			var v = jQuery(this).val();
			
			if (v == "") {
				jQuery("select#productversion").html("").attr("disabled","disabled");	
			}
			versionPopulate(v);
	
	  	});
		
		refreshTopic = function(){
			var version = jQuery("select#productversion").val();
			var family  = jQuery("select#productline").val();
			topicPopulate(family, version);
	
	  	};
		
		jQuery("select#productversion").change(refreshTopic);
		jQuery("select#productline").change(refreshTopic);
		
		jQuery("#contentTypeKB").click(function(){
			
			if (this.checked==true)
			{
				jQuery("select#topic").removeAttr("disabled");
			}else
			{
				jQuery("select#topic").attr("disabled","disabled");
				
			}
	  	});
		
		jQuery("#contentTypeKB").load(function(){
			
			if (this.checked==true)
			{
				jQuery("select#topic").removeAttr("disabled");
			}else
			{
				jQuery("select#topic").attr("disabled","disabled");
				
			}
	  	});
	  	
		jQuery(".tabcontrol > a").click(function(){
			if (jQuery("#searchterm").val()!="") {
				jQuery(".tabcontrol > a").attr("href",function(){
					return jQuery(".tabcontrol > a").attr("href") + "?qs=" + escape(jQuery("#searchterm").val());
				});
			}
		});
		var args = location.search.substring(1,location.search.length).split(',');
		for (var i=0;i<args.length;i++){
			var argI = args[i].split('=');
			if (unescape(argI[0])=='qs'){
				jQuery("#searchterm").val(unescape(argI[1]));
			}
		}
	
		function versionPopulate(v) {
			
			if (v!="") {
				jQuery.getJSON("/search/json?family="+v, function(j){	
					var f = j.versionlist;
					
					jQuery("select#productversion").html("").attr("disabled","disabled");
					jQuery("#productversion").addOption("", "(All Versions)", true);
					for (var i = 0; i < f.length; i++)
					{
						jQuery("#productversion").addOption(f[i],f[i],false);
					}
					
					jQuery('#productversion').selectOptions(currVersion).attr('selected', 'selected');
					jQuery("select#productversion").removeAttr("disabled" );				
				});
			}
	    }
		
		function topicPopulate(family, version) {

			var jsonUrl = "/search/topic/json";

			if (family!="" && family!=null && version!="" && version!=null) {
				jsonUrl = jsonUrl + "?family="+family+"&version="+version
			}else if (family!="" && family!=null) {
				jsonUrl = jsonUrl + "?family="+family;
			}

			jQuery.getJSON(jsonUrl, function(j){	
				var topicList = j.topicList;
				jQuery("select#topic").html("");
				jQuery("#topic").addOption("", "(All Topics)", true);
				
				for (var i = 0; i < topicList.length; i++)
				{
					jQuery("#topic").addOption(topicList[i],topicList[i],false);
				}

			});

	    }
	}
	if (isForumSearch) {
		/*=======================================================================
		jquery_afsearch.js
		=======================================================================*/
		searchInitialize("searchhelpadvanced");
		transferQueryString();
		
		jQuery("[@name=submitter]").click(function(){
			if (jQuery("#searchterm").val().length < 1) {
				jQuery("#assearchquerywarning").css({ display: "inline" });
			}
			else if (jQuery("#searchterm").val().length >= 1) {
				jQuery("#searchform").submit();
			}
		});
			
		jQuery("#searchform").submit(function(){
			if ((jQuery("#searchterm").val().length == 0) && (jQuery("#displayterm").val().length == 0)) {
				jQuery("#assearchquerywarning").css({ display: "inline" });
				return false;
			}
			if ((jQuery("#searchterm").val().length == 0) && (jQuery("#displayterm").val().length > 0)) {
				jQuery("#searchterm").val('*');
			}
		});
		jQuery("#searchterm").focus(function(){
			jQuery("#assearchquerywarning").css({ display: "none" });		
		});
		if (currForum.length > 0) {
			jQuery("#parentCatID1").selectOptions(currForum).attr('selected', 'selected');
			jQuery(".tabcontrol > a").click(function(){
				if (jQuery("#searchterm").val()!="") {
					jQuery(".tabcontrol > a").attr("href",function(){
						return jQuery(".tabcontrol > a").attr("href") + "?qs=" + escape(jQuery("#searchterm").val());
					});
				}
			});
		}
		var args = location.search.substring(1,location.search.length).split(',');
		for (var i=0;i<args.length;i++){
			var argI = args[i].split('=');
			if (unescape(argI[0])=='qs') {
				jQuery("#searchterm").val(unescape(argI[1]));
			}
			if (unescape(argI[0])=='fId') {
				jQuery("#parentCatID1").selectOptions(unescape(argI[1]));
			}
		}
		
	    	//Transfer qs back to basic search
	   	jQuery("#asbasicsearchlink").click(function(){
			if (jQuery("#searchterm").val()!="") {
				jQuery("#asbasicsearchlink").attr("href",function(){
					return jQuery("#asbasicsearchlink").attr("href") + "?qs=" + escape(jQuery("#searchterm").val());
				});
			}
		});
	}

	if(hasTwoColList) {
		/*=======================================================================
		inline from product version FTL
		=======================================================================*/
		jQuery(".ctxtstabslisttwocol").twoColumnList();
	}

	if (hasTabs) {
	
		var tabsPosition = jQuery("#ctxkcmain")
		var offset = tabsPosition.offset();
		var tabsPositionTop = offset.top;
 
		setTimeout('jQuery("#ctxkcmain").scrollTop(0);',1000);
		jQuery(window).scrollTop(0);
		//jQuery("#ctxkcmain").scrollTop(0);
		//jQuery(window).scrollTop(tabsPositionTop);


	
		/*=======================================================================
		inline from product version FTL
		=======================================================================*/
		jQuery(".langSubmit").css("display","none");
		jQuery("#container-1").tabs();
		jQuery(".langSelect").bind("change", function(){
			jQuery(this).parent().submit();
	  	});
	  	jQuery("#container-loader").css("display","none");
	  	jQuery("#container-1").css("display","inline");
	  	jQuery("#container-1").attr("class","");

		/* THIS CODE KEEPS YOU ON THE TAB YOU WERE VIEWING WHEN YOU CHANGED LANGUAGES */
		var currentUrl = window.location.href
		if (currentUrl.search("tab-hotfix") != -1) {
			jQuery("#container-1").triggerTab("tab-hotfix");
		}
		if (currentUrl.search("tab-doc") != -1) {
			jQuery("#container-1").triggerTab("tab-doc");
		}
		if (currentUrl.search("tab-comp") != -1) {
			jQuery("#container-1").triggerTab("tab-comp");
		}
	  	
	  	
		/*=======================================================================
		from tabs.mozilla.js
		=======================================================================*/
	  	
		if(jQuery.browser.mozilla && location.hash.length>0) {
			jQuery("#ctxkcnav").css("padding-top","15px");
			jQuery("#ctxkccontent").css("padding-top","15px");
			jQuery("#ctxkcutil").css("padding-top","15px");
		}	  	
	}
	
	if (hasComments) {
		/* VALIDATING THAT COMMENT FIELD HAS TEXT IN IT */
		jQuery("[@name=post]").click(function(){
			if (jQuery("#commentInput").val().length < 1)  {
				jQuery("#commentInput").css("background-color","#ffecbf");
				alert("Please type in a comment.");	
			}
			else if (jQuery("#commentInput").val().length >= 1)  {
				jQuery("#commentInput").css("background-color","#ffffff");
				jQuery("form[@name=commentForm]").submit();
			}
			return false;
		});
	
	
		jQuery("#commentAdd").css({ display: "block"});
		jQuery("#commentFormWrapper").hide();
		jQuery("#commentAddNoLink").hide();
		jQuery("#commentAdd").show().click(function() { 
			jQuery("#commentFormWrapper").show();
			jQuery("#commentAddNoLink").show();
			jQuery(this).hide();		
			return false; 
		});
		jQuery("#commentForm").bind("reset", function() {
			jQuery("#commentFormWrapper").hide();
			jQuery("#commentAddNoLink").hide();
			jQuery("#commentAdd").show();
			
		});
		jQuery("#btnspellcheck").css("display","inline");
	}

	if (isDocFeedbackArticle) {
		jQuery("[@name=submitDocFeedback]").click(function(){
					
			/* VALIDATING REQUIRED TEXT FIELDS  */
			
			if (jQuery("[@name=kbDocTitle]").val().length < 1)  {
				jQuery("[@name=kbDocTitle]").css("background-color","#ffecbf");
				var kbDocTitlefield = 0;
				setTimeout('jQuery("[@name=kbDocTitle]").focus();',100);
			}
			else if (jQuery("[@name=kbDocTitle]").val().length >= 1)  {
				jQuery("[@name=kbDocTitle]").css("background-color","#ffffff");
				var kbDocTitlefield = 1;
				var kbDocTitletext = ""
			}
			if (jQuery("[@name=ctxNumber]").val().length < 1)  {
				jQuery("[@name=ctxNumber]").css("background-color","#ffecbf");
				var ctxNumberfield = 0;
				setTimeout('jQuery("[@name=ctxNumber").focus();',100);
			}
			else if (jQuery("[@name=ctxNumber]").val().length >= 1)  {
				jQuery("[@name=ctxNumber]").css("background-color","#ffffff");
				var ctxNumberfield = 1;
				var ctxNumbertext = ""
			}
			if (jQuery("[@name=docURL]").val().length < 1)  {
				jQuery("[@name=docURL]").css("background-color","#ffecbf");
				var docURLfield = 0;
				setTimeout('jQuery("[@name=docURL").focus();',100);
			}
			else if (jQuery("[@name=docURL]").val().length >= 1)  {
				jQuery("[@name=docURL]").css("background-color","#ffffff");
				var docURLfield = 1;
				var docURLtext = ""
			}
			if (jQuery("[@name=comment]").val().length < 1)  {
				jQuery("[@name=comment]").css("background-color","#ffecbf");
				var commentfield = 0;
				setTimeout('jQuery("[@name=comment").focus();',100);
			}
			else if (jQuery("[@name=comment]").val().length >= 1)  {
				jQuery("[@name=comment]").css("background-color","#ffffff");
				var commentfield = 1;
				var commenttext = ""
			}
			
			/* PUTTING FIELD NAMES IN ALERT MESSAGE AREA */
			if (kbDocTitlefield == 0 ){
				var kbDocTitletext = "- Document Title<br/>"
			}
			if (ctxNumberfield == 0 ) {
				var ctxNumbertext = "- Document ID<br/>"
			}
			if (docURLfield == 0 ) {
				var docURLtext = "- Document URL<br/>"
			}
			if (commentfield == 0 ) {
				var commenttext = "- Enter Your Comment"
			}
			
			/* MAKING VISIBLE AND HIDING ALERT MESSAGE AREA */
			if (kbDocTitlefield == 0 || ctxNumberfield == 0 || docURLfield == 0 || commentfield == 0 ) {
				jQuery("#reqfields2").removeClass("fderrormsgshow").addClass("fderrormsg");
				var requiredfields = kbDocTitletext + ctxNumbertext + docURLtext + commenttext;
				jQuery("#reqfieldname2").html(requiredfields);
			}
			else if (kbDocTitlefield == 1 && ctxNumberfield == 1 && docURLfield == 1 && commentfield == 1 ) {
				jQuery("#reqfields2").removeClass("fderrormsg").addClass("fderrormsgshow");
				jQuery("form[@name=documentationForm]").submit();
			}
			return false;
		});	
	}		
	
	if (isDocFeedbackHelp) {
		jQuery("[@name=submitDocFeedback]").click(function(){
						
				/* VALIDATING REQUIRED TEXT FIELDS  */
				
				if (jQuery("[@name=productName]").val().length < 1)  {
					jQuery("[@name=productName]").css("background-color","#ffecbf");
					var productNamefield = 0;
					setTimeout('jQuery("[@name=productName]").focus();',100);
				}
				else if (jQuery("[@name=productName]").val().length >= 1)  {
					jQuery("[@name=productName]").css("background-color","#ffffff");
					var productNamefield = 1;
					var productNametext = ""
				}
				if (jQuery("[@name=productVersion]").val().length < 1)  {
					jQuery("[@name=productVersion]").css("background-color","#ffecbf");
					var productVersionfield = 0;
					setTimeout('jQuery("[@name=productVersion").focus();',100);
				}
				else if (jQuery("[@name=productVersion]").val().length >= 1)  {
					jQuery("[@name=productVersion]").css("background-color","#ffffff");
					var productVersionfield = 1;
					var productVersiontext = ""
				}
				if (jQuery("[@name=productLanguage]").val().length < 1)  {
					jQuery("[@name=productLanguage]").css("background-color","#ffecbf");
					var productLanguagefield = 0;
					setTimeout('jQuery("[@name=productLanguage").focus();',100);
				}
				else if (jQuery("[@name=productLanguage]").val().length >= 1)  {
					jQuery("[@name=productLanguage]").css("background-color","#ffffff");
					var productLanguagefield = 1;
					var productLanguagetext = ""
				}
				if (jQuery("[@name=helpSystemName]").val().length < 1)  {
					jQuery("[@name=helpSystemName]").css("background-color","#ffecbf");
					var helpSystemNamefield = 0;
					setTimeout('jQuery("[@name=helpSystemName").focus();',100);
				}
				else if (jQuery("[@name=helpSystemName]").val().length >= 1)  {
					jQuery("[@name=helpSystemName]").css("background-color","#ffffff");
					var helpSystemNamefield = 1;
					var helpSystemNametext = ""
				}
				if (jQuery("[@name=helpSystemTopicTitle]").val().length < 1)  {
					jQuery("[@name=helpSystemTopicTitle]").css("background-color","#ffecbf");
					var helpSystemTopicTitlefield = 0;
					setTimeout('jQuery("[@name=helpSystemTopicTitle").focus();',100);
				}
				else if (jQuery("[@name=helpSystemTopicTitle]").val().length >= 1)  {
					jQuery("[@name=helpSystemTopicTitle]").css("background-color","#ffffff");
					var helpSystemTopicTitlefield = 1;
					var helpSystemTopicTitletext = ""
				}
				if (jQuery("[@name=comment]").val().length < 1)  {
					jQuery("[@name=comment]").css("background-color","#ffecbf");
					var commentfield = 0;
					setTimeout('jQuery("[@name=comment").focus();',100);
				}
				else if (jQuery("[@name=comment]").val().length >= 1)  {
					jQuery("[@name=comment]").css("background-color","#ffffff");
					var commentfield = 1;
					var commenttext = ""
				}
				
				/* PUTTING COMMAS BETWEEN FIELD NAMES IN ALERT MESSAGE AREA */
				if (productNamefield == 0 ){
					var productNametext = "- Choose a Product<br/>"
				}
				if (productVersionfield == 0 ) {
					var productVersiontext = "- Choose a Version<br/>"
				}
				if (productLanguagefield == 0 ) {
					var productLanguagetext = "- Choose a Language<br/>"
				}
				if (helpSystemNamefield == 0 ) {
					var helpSystemNametext = "- Name of Component, Console, or Tool<br/>"
				}
				if (helpSystemTopicTitlefield == 0 ) {
					var helpSystemTopicTitletext = "- Topic Title<br/>"
				}
				if (commentfield == 0 ) {
					var commenttext = "- Enter Your Comment"
				}
				
				/* MAKING VISIBLE AND HIDING ALERT MESSAGE AREA */
				if (productNamefield == 0 || productVersionfield == 0 || productLanguagefield == 0 || helpSystemNamefield == 0 || helpSystemTopicTitlefield == 0 || commentfield == 0 ) {
					jQuery("#reqfields2").removeClass("fderrormsgshow").addClass("fderrormsg");
					var requiredfields = productNametext + productVersiontext + productLanguagetext + helpSystemNametext + helpSystemTopicTitletext + commenttext;
					jQuery("#reqfieldname2").html(requiredfields);
				}
				else if (productNamefield == 1 && productVersionfield == 1 && productLanguagefield == 1 && helpSystemNamefield == 1 && helpSystemTopicTitlefield == 1 && commentfield == 1 ) {
					jQuery("#reqfields2").removeClass("fderrormsg").addClass("fderrormsgshow");
					jQuery("form[@name=documentationForm]").submit();
				}
				return false;
			});	
	}
	
	if (isDocFeedbackPDF) {

		jQuery("[@name=submitDocFeedback]").click(function(){
						
			/* VALIDATING REQUIRED TEXT FIELDS  */
				
				if (jQuery("[@name=productName]").val().length < 1)  {
					jQuery("[@name=productName]").css("background-color","#ffecbf");
					var productNamefield = 0;
					setTimeout('jQuery("[@name=productName]").focus();',100);
				}
				else if (jQuery("[@name=productName]").val().length >= 1)  {
					jQuery("[@name=productName]").css("background-color","#ffffff");
					var productNamefield = 1;
					var productNametext = ""
				}
				if (jQuery("[@name=productVersion]").val().length < 1)  {
					jQuery("[@name=productVersion]").css("background-color","#ffecbf");
					var productVersionfield = 0;
					setTimeout('jQuery("[@name=productVersion").focus();',100);
				}
				else if (jQuery("[@name=productVersion]").val().length >= 1)  {
					jQuery("[@name=productVersion]").css("background-color","#ffffff");
					var productVersionfield = 1;
					var productVersiontext = ""
				}
				if (jQuery("[@name=productLanguage]").val().length < 1)  {
					jQuery("[@name=productLanguage]").css("background-color","#ffecbf");
					var productLanguagefield = 0;
					setTimeout('jQuery("[@name=productLanguage").focus();',100);
				}
				else if (jQuery("[@name=productLanguage]").val().length >= 1)  {
					jQuery("[@name=productLanguage]").css("background-color","#ffffff");
					var productLanguagefield = 1;
					var productLanguagetext = ""
				}
				if (jQuery("[@name=documentType]").val().length < 1)  {
					jQuery("[@name=documentType]").css("background-color","#ffecbf");
					var documentTypefield = 0;
					setTimeout('jQuery("[@name=documentType").focus();',100);
				}
				else if (jQuery("[@name=documentType]").val().length >= 1)  {
					jQuery("[@name=documentType]").css("background-color","#ffffff");
					var documentTypefield = 1;
					var documentTypetext = ""
				}
				if (jQuery("[@name=chapterTitle]").val().length < 1)  {
					jQuery("[@name=chapterTitle]").css("background-color","#ffecbf");
					var chapterTitlefield = 0;
					setTimeout('jQuery("[@name=chapterTitle").focus();',100);
				}
				else if (jQuery("[@name=chapterTitle]").val().length >= 1)  {
					jQuery("[@name=chapterTitle]").css("background-color","#ffffff");
					var chapterTitlefield = 1;
					var chapterTitletext = ""
				}
				if (jQuery("[@name=pageNumber]").val().length < 1)  {
					jQuery("[@name=pageNumber]").css("background-color","#ffecbf");
					var pageNumberfield = 0;
					setTimeout('jQuery("[@name=pageNumber").focus();',100);
				}
				else if (jQuery("[@name=pageNumber]").val().length >= 1)  {
					jQuery("[@name=pageNumber]").css("background-color","#ffffff");
					var pageNumberfield = 1;
					var pageNumbertext = ""
				}
				if (jQuery("[@name=comment]").val().length < 1)  {
					jQuery("[@name=comment]").css("background-color","#ffecbf");
					var commentfield = 0;
					setTimeout('jQuery("[@name=comment").focus();',100);
				}
				else if (jQuery("[@name=comment]").val().length >= 1)  {
					jQuery("[@name=comment]").css("background-color","#ffffff");
					var commentfield = 1;
					var commenttext = ""
				}
				
				/* PUTTING COMMAS BETWEEN FIELD NAMES IN ALERT MESSAGE AREA */
				if (productNamefield == 0 ){
					var productNametext = "- Choose a Product<br/>"
				}
				if (productVersionfield == 0 ) {
					var productVersiontext = "- Choose a Version<br/>"
				}
				if (productLanguagefield == 0 ) {
					var productLanguagetext = "- Choose a Language<br/>"
				}
				if (documentTypefield == 0 ) {
					var documentTypetext = "- Document Type<br/>"
				}
				if (chapterTitlefield == 0 ) {
					var chapterTitletext = "- Chapter Title<br/>"
				}
				if (pageNumberfield == 0 ) {
					var pageNumbertext = "- Page Number<br/>"
				}
				if (commentfield == 0 ) {
					var commenttext = "- Enter Your Comment"
				}
				
				/* MAKING VISIBLE AND HIDING ALERT MESSAGE AREA */
				if (productNamefield == 0 || productVersionfield == 0 || productLanguagefield == 0 || documentTypefield == 0 || chapterTitlefield == 0 || pageNumberfield == 0 || commentfield == 0 ) {
					jQuery("#reqfields2").removeClass("fderrormsgshow").addClass("fderrormsg");
					var requiredfields = productNametext + productVersiontext + productLanguagetext + documentTypetext + chapterTitletext + pageNumbertext + commenttext;
					jQuery("#reqfieldname2").html(requiredfields);
				}
				else if (productNamefield == 1 && productVersionfield == 1 && productLanguagefield == 1 && documentTypefield == 1 && chapterTitlefield == 1 && pageNumberfield == 1 && commentfield == 1 ) {
					jQuery("#reqfields2").removeClass("fderrormsg").addClass("fderrormsgshow");
					jQuery("form[@name=documentationForm]").submit();
				}
				return false;
			});	
	
	}
	
	if (hasHoverTip) {
		jQuery(".hashover").each(function(){
			jQuery(this).css("cursor","help");
			var thistiptemplate = jQuery("#" + jQuery(this).attr("id") + "_tip");
			var thistip = thistiptemplate.clone();
			var thistipid = thistiptemplate.attr("id")+"x";
			thistip.attr("id",thistipid);
			var r = 180;
			var g = 223;
			var b = 111;
			var steps = 15;
			var speed = 4000; // 3 seconds
			var stepspeed = speed/steps;
			var delay = 500;
			var rs = (255-r)/steps;
			var gs = (255-g)/steps;
			var bs = (255-b)/steps;
			
			jQuery(this).hover(
				function(){
					jQuery(this).before(thistip);
					thistip.css("display","inline");
					thistip.css("opacity","0.0");
					thistip.css("top",(thistip.height()*-1)-12+"px");
					for (var i=0.1;i<=1.0;i=i+0.1) {
						setTimeout('jQuery("#'+thistipid+'").css("opacity","'+rounddecimals(i,1)+'");',Math.round(600*i));
					}
					// if we want the 'Products Nav' to flash, disable comments on the below:
					/*for (var i=0;i<=steps;i++) {
						rc = Math.round(r+(rs*i));
						gc = Math.round(g+(gs*i));
						bc = Math.round(b+(bs*i));
						setTimeout("jQuery('#ctxkcnav dl').css('background-color','rgb("+rc+","+gc+","+bc+")');",(stepspeed*i)+delay);
					}*/

				},
				function(){
					thistip.remove();
				}
			);
		});
	}
	
});


/*=======================================================================
FUNCTIONS
=======================================================================*/
function rounddecimals(num, places) {
    var result1 = num * Math.pow(10, places);
    var result2 = Math.round(result1);
    var result3 = result2 / Math.pow(10, places);
    return zeropad(result3, places);
}
function zeropad(rounded_value, decimal_places) {
    var value_string = rounded_value.toString();
    var decimal_location = value_string.indexOf(".");
    if (decimal_location == -1) {
        decimal_part_length = 0;
        value_string += decimal_places > 0 ? "." : "";
    }
    else {
        decimal_part_length = value_string.length - decimal_location - 1;
    }
    var pad_total = decimal_places - decimal_part_length;
    if (pad_total > 0) {
        for (var counter = 1; counter <= pad_total; counter++) 
            value_string += "0";
        }
    return value_string;
}


/*=======================================================================
loginvalidation.js
This method performs validation only
=======================================================================*/

var status;
function validate(field1, field2){
		var user1 = field1.val();
		var username1 = field1.attr("name");
		var pwd1 = field2.val();
		var pwdname1 = field2.attr("name");
		var formObj = document.forms["loginForm"];
		
		if ((user1 =="") || (user1 ==" ") || (pwd1 =="") || (pwd1 ==" ")){
			jQuery("div#warningfield").removeClass("message").addClass("errormessage");
			formObj.elements["username"].value ="";
			formObj.elements["password"].value ="";
			formObj.elements["username"].focus();
			status = false;
		}
		else {
			var response = "";
			var jsonStatus;
			status = true;
		}
	return status;
}

if (hasComments) {

    function spellForm() {
        document.commentForm.action='/kc/spellComment';
        document.commentForm.submit();
        return false;
    }
                    
		
	function styleTag(tag, ta) {
	    var tagOpen = '<' + tag.toLowerCase() + '>';
	    var tagClose = '</' + tag.toLowerCase() + '>';
	    if (document.selection && document.selection.createRange() &&
	            document.selection.createRange().parentElement().tagName == 'TEXTAREA')
	    {
	        var selected = document.selection.createRange().text;
	        if (selected) {
	            var addSpace = false;
	            if (selected.charAt(selected.length-1) == ' ') {
	                selected = selected.substring(0, selected.length-1);
	                addSpace = true;
	            }
	            document.selection.createRange().text
	                    = tagOpen + selected + tagClose + ((addSpace)?" ":"");
	        } else {
	            ta.value += tagOpen + tagClose;
	        }
	    }
	    else if (ta.selectionStart && ta.selectionEnd) {
	        ta.focus();
	        var selStart = ta.selectionStart;
	        var selEnd = ta.selectionEnd;
	        var textSize = ta.textLength;
	        var newValue = new String(ta.value);
	        var selection = newValue.substring(selStart, selEnd);
	        var addSpace = false;
	
	        if (selection.charAt(selection.length-1) == ' ') {
	            selection = selection.substring(0, selection.length-1);
	            addSpace = true;
	        }
	
	        newValue = newValue.substring(0, selStart) +
	                   tagOpen +
	                   selection +
	                   ((addSpace)?" ":"") +
	                   tagClose +
	                   newValue.substring(selEnd, textSize);
	
	        ta.value = newValue;
	    }
	    else {
	        ta.value += tagOpen + tagClose;
	    }
	    ta.focus();
	    return;
	}	
}


if (isGlobal) {
	/*=======================================================================
	from searchGSAmacro.ftl
	=======================================================================*/
	
	function gsaval(thisfield){
	
				var fieldval = thisfield.val();
				var fieldname = thisfield.attr("name");
				var thisForm = document.forms["searchkc"];
	
				if ((fieldval =="") || (fieldval ==" ")){
					window.location.href = "/search/basic/";
					result = false;
				}
				else {
					result = true;
					}
			return result;		
	}
}

if (hasTwoColList) {

	jQuery.fn.twoColumnList = function(favorLeft) { // called on list's parent container
		if (favorLeft==null) { favorLeft = false; }
		var innerList;
		var listType;
		var minPercentage = 2.0;
		if ( this.find("ol").length == 1 ) {
			innerList = this.find("ol");
			listType = "ol"; 
		}
		else if ( this.find("ul").length == 1) { 
			innerList = this.find("ul");
			listType = "ul";
		}
		else {
			return false;
		}
		innerElements = innerList.find("li");
		if (innerElements.length != 1) { 
			var firstListCount = Math.ceil(innerElements.length/2);
			var firstElemSet = innerElements.slice(0,firstListCount);
			var secondElemSet = innerElements.slice(firstListCount);
			jQuery(innerList.remove());
			var firstElemList = jQuery("<"+listType+" id='firstlist'></"+listType+">").appendTo(this);
			var secondElemList = jQuery("<"+listType+" id='secondlist'></"+listType+">").appendTo(this);
			firstElemSet.appendTo(firstElemList);
			secondElemSet.appendTo(secondElemList);
		
			secondElemList.attr("start",firstListCount+1);
			firstElemList.wrap("<div class='listcolumn' id='firstcolumn'>");
			secondElemList.wrap("<div class='listcolumn' id='secondcolumn'>");
			jQuery("<div style='clear: both;'></div>").appendTo(this);
			var fY = jQuery("#firstcolumn").height();
			var sY = jQuery("#secondcolumn").height();
			var sP = Math.ceil(100 - ((100/fY) * sY));
			var firstCol = jQuery("#firstcolumn");
			var secondCol = jQuery("#secondcolumn");
			var dirChange = 0;
			var dir;
			while ((sP >= minPercentage)||(sP <= (-1 * minPercentage))) {
				//alert('resizing ' + sP);
				if (sP >= minPercentage) { //we move from left col to right col
					jQuery("#secondlist li:first").before(jQuery("#firstlist li:last").remove());
					// alert('moved first from 2nd to last of 1st');
					if (dir == -1) { dirChange++; }
					dir = 1;
					/* if (!favorLeft) {
						if (dirChange > 3) { break; }
					}*/
				}
				else { // right col to left col
					jQuery("#firstlist li:last").after(jQuery("#secondlist li:first").remove());
					// alert('moved last from 1st to first of 2nd');
					if (dir == 1) { dirChange++; }
					dir = -1;
					if (dirChange > 3) { break; }
				}
				fY = jQuery("#firstcolumn").height();
				sY = jQuery("#secondcolumn").height();
				sP = Math.ceil(100 - ((100/fY) * sY));
			}
			if (favorLeft && (fY < sY)) {
				jQuery("#firstlist li:last").after(jQuery("#secondlist li:first").remove());
			}
			var newLength = jQuery("#firstlist > li").size();
			jQuery("#secondlist").attr("start",newLength+1);
		}
	};


}

if (isSearch || isAdvancedSearch || isForumSearch) {
	/*=======================================================================
	from jquery_search.js
	=======================================================================*/
	function transferQueryString() {
	
		jQuery("#advancedsearchlink").click(function(){
			if (jQuery("#searchterm").val()!="") {
				jQuery("#advancedsearchlink").attr("href",function(){
					return jQuery("#advancedsearchlink").attr("href") + "?qs=" + escape(jQuery("#searchterm").val());
				});
			}
		});
		var args = location.search.substring(1,location.search.length).split(',');
		for (var i=0;i<args.length;i++){
			var argI = args[i].split('=');
			if (unescape(argI[0])=='qs'){
				jQuery("#searchterm").val(unescape(argI[1]));
			}
		}
	}
	
	function searchInitialize(helpSource) {
	
		jQuery(".searchhelplinkwrapper").css("display","inline");	
		jQuery("#searchform").submit(function(){
			if (jQuery("#searchterm").val().length == 0) {
			   
				jQuery("#assearchquerywarning").css({ display: "inline" });
				return false;
			}
		});
		jQuery("[@name=submitter]").click(function() {
			if (jQuery("#searchterm").val().length < 1) {
				jQuery("#assearchquerywarning").css({ display: "inline" });
			}
			else if (jQuery("#searchterm").val().length >= 1) {
				jQuery("#searchform").submit();
			}
		});
		
		jQuery("#searchterm").focus(function(){
			jQuery("#assearchquerywarning").css({ display: "none" });	
		});
		jQuery(".searchhelpopenerlink").click(function(){
			window.open('/kc/popupsearch/search/', 'KnowledgeCenterSearchHelp' ,'height=545,width=750,left=250,top=250,scrollbars=0,resizable=0,status=0,menubar=0,toolbar=0,titlebar=0');
			//var x = (jQuery(window).width()/2)-(jQuery("#searchhelpwrapper").width()/2);
			//var y = (jQuery(window).height()/2)-(jQuery("#searchhelpwrapper").height()/2);
			//if (x < 0) { x = 0; }
			//if (y < 0) { y = 0; }
			//jQuery("#searchhelpwrapper").css("left", x+"px").css("top", y+"px");
			//jQuery("#searchhelpwrapper").css("display","inline");
			//jQuery("#searchhelpfader").css("display","inline");
			//return false;
		});
		
		jQuery(".searchhelpcloselink").click(closeBox);
	
		function closeBox() {
			jQuery("#searchhelpwrapper").css("display","none");
			jQuery("#searchhelpfader").css("display","none");
			jQuery("#searchhelpmain").css("display","block");
			return false;
		}
	
	    loadIntoHelp(helpSource);
	
	    jQuery("#searchhelpmain").find("a").each(function(){
	    	jQuery(this).click(function(){
		    	loadIntoHelp(jQuery(this).attr('href'));
		    	return false;
		   	});
		});
	    jQuery(".searchhelpnavlinkblur > a").click(function(){
	    	return false;
	    });
	    jQuery(".searchhelpnavimgblur > a").click(function(){
	    	return false;
	    });	
	
		jQuery(window).bind("resize", function(){ 
			jQuery("#searchhelpfader").css("width", jQuery(document).width()+"px").css("height", jQuery(document).height()+"px");		
		});
		
		   	//Listen to escape event
	   	jQuery(".searchhelplinkwrapper").keydown(function(event) { 
			
			var key = event.charCode ? event.charCode: event.keyCode ? event.keyCode : 0;
			if ( key == 27 ) {
				closeBox();
			}
		});
	}


	
	function loadIntoHelp(toLoad) {
	    jQuery(".searchhelpnavlinkblur").removeClass().addClass("searchhelpnavlink");
	   	jQuery(".searchhelpnavimgblur").removeClass().addClass("searchhelpnavimg");
	   	jQuery("#searchhelpnavigation").find("a").each(function(){
	   		if (jQuery(this).attr('href')==toLoad) {
	   			var ourParent = jQuery(this).parent();
		   		if (ourParent.attr("class") == "searchhelpnavimg") {
		   			ourParent.removeClass().addClass("searchhelpnavimgblur");
		   		}
		   		else {
		   			ourParent.removeClass().addClass("searchhelpnavlinkblur");
		   		}
	   		}
	   	});
	   	jQuery(".searchhelpcontentoff").removeClass().addClass("searchhelpcontentoff");
	   	jQuery("#"+toLoad).addClass("searchhelpcontenton");
	}

}