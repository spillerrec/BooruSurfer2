var items = window.location.pathname.split("/")
items.shift()

var strictParseInt = function( str ) {
	if( isNaN( String( str ) * 1 ) )
		return NaN
	return parseInt( str );
}

var site = items[1];
var page_parsed = strictParseInt( items[2] );
var page = isNaN(page_parsed) ? 1 : page_parsed;
var limit = -1; //TODO: implement
var search = isNaN(page_parsed) ? items[2] : items[3];

var index_url = function() { return "/jsindex/" + site + "/" + (page+1) + "/" + search; }

var load_next = function() {
//	alert( "loading next" );
	$.get( index_url(), function( response ){
		$( "#container .post_list" ).append( response );
		page += 1;
	});
}

window.onload = function() {
	
	$.get( index_url(), function( response ) {
			$( "nav.page_nav" ).remove();
			$( "#container .post_list" ).append( response );
			page += 1;

			$(window).scroll(function(){
				var current = $(window).scrollTop() + window.innerHeight;
				var total = $(document).height();
				if(Math.round(current) >= total) {
					load_next();
				}
			});
		} );
	
	$("#search").parent().append('<div id="search_list"></div>');
	$("#search").attr("autocomplete", "off");
	
	$("#search").on("input", function(){
		var value = $("#search").val();
		$.getJSON( "/tags/" + site + "/" + value, function(data){
			var ul = $("<div>", {id: "search_list"});
			
			data.forEach(function(item){
					var name = item["id"];
					var li = $('<li>')
					var add = $('<a>', {"href":"/index/" + site + "/" + search + "+" + name});
					add.append("+");
					var sub = $('<a>', {"href":"/index/" + site + "/" + search + "+-" + name});
					sub.append("-");
					var a = $('<a>', {"class": "tagtype" + item['type'], "href":"/index/" + site + "/" + name});
					var span = $('<span>(' + item['count'] + ')</span>');
					a.append(name);
					a.append(span);
					li.append(add);
					li.append(sub);
					li.append(a);
					ul.append(li);
				});
			$("#search_list").replaceWith(ul);
		} );
	});
}
