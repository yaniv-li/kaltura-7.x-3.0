/**
 * jquery.scrollable 0.13. Put your HTML scroll.
 * 
 * http://flowplayer.org/tools/scrollable.html
 *
 * Copyright (c) 2008 Tero Piirainen (support@flowplayer.org)
 *
 * Released under the MIT License:
 * http://www.opensource.org/licenses/mit-license.php
 * 
 * >> Basically you can do anything you want but leave this header as is <<
 *
 * Since  : 0.10 - 03/01/2008
 * Version: 0.13 - Wed Nov 05 2008 12:04:04 GMT-0000 (GMT+00:00)
 */
(function($) {
		
	// constructor
	function Scrollable(el, config) {   
		
		// current instance
		var self = this;  
		
		if (!Scrollable.current) {
			Scrollable.current = this;	
		}
		
		var opts = {								
			size: 5,
			vertical:false,				
			activeClass:'active',
			speed: 300,
			onSeek: null,
			clickable: true,
			
			// jquery selectors
			items: '.items',
			prev:'.prev',
			next:'.next',
			navi:'.navi',
			naviItem:'span', 
			loop: false
		}; 

		
		this.opts = $.extend(opts, config);  
		this.opts.horizontal = !opts.vertical;
		
		// root / itemRoot
		this.root = $(el);
		var root = this.root;
		var itemRoot = $(opts.items, root);			
		if (!itemRoot.length)  { itemRoot = root; }  
		
		
		// wrap itemRoot.children() inside container
		itemRoot.css({position:'relative', overflow:'hidden', visibility:'visible'});
		itemRoot.children().wrapAll('<div class="__scrollable" style="position:absolute"/>'); 
		
		this.wrap = itemRoot.find(":first");
		this.wrap.css(opts.horizontal ? "width" : "height", "200000em").after('<br clear="all" />');		
		this.items = this.wrap.children();
		this.index = 0;

		
		// set dimensions based on offsets of the two first elements
		if (opts.horizontal) {
			itemRoot.width(opts.size * (this.items.eq(1).offset().left - this.items.eq(0).offset().left) -2);	
		} else {
			itemRoot.height(opts.size * (this.items.eq(1).offset().top - this.items.eq(0).offset().top) -2);	
		} 

		// mousewheel
		if ($.isFunction($.fn.mousewheel)) {
			root.bind("mousewheel.scrollable", function(e, delta)  { 
				self.move(-delta, 50);		
				return false;
			});
		}  
		
		// item.click()
		if (opts.clickable) {
			this.items.each(function(index, arg) {
				$(this).bind("click.scrollable", function() {
					self.click(index);		
				});
			});				
		}


		this.activeIndex = 0;
		
		// prev
		$(opts.prev, root).click(function() { 
			self.prev(); 
		});
		

		// next
		$(opts.next, root).click(function() { 
			self.next(); 
		});
		

		// navi 			
		$(opts.navi, root).each(function() { 				
			var navi = $(this);
			
			var status = self.getStatus();
			
			// generate new entries
			if (navi.is(":empty")) {
				for (var i = 0; i < status.pages; i++) {		
					
					var item = $("<" + opts.naviItem + "/>").attr("page", i).click(function(e) {							
						var el = $(this);
						el.parent().children().removeClass(opts.activeClass);
						el.addClass(opts.activeClass);
						self.setPage(el.attr("page"));
						e.preventDefault();
					});
					
					if (i === 0) { item.addClass(opts.activeClass); }
					navi.append(item);					
				}
				
			// assign onClick events to existing entries
			} else {
				
				// find a entries first -> syntaxically correct
				var els = navi.find("a");
				
				if (!els.length) { 
					els = navi.children(); 
				}
				
				els.each(function(i)  {
					var item = $(this);
					item.attr("page", i);
					if (i === 0) { item.addClass(opts.activeClass); }
					
					item.click(function() {
						navi.find("." + opts.activeClass).removeClass(opts.activeClass);
						item.addClass(opts.activeClass);
						self.setPage(item.attr("page"));
					});
					
				});
			}
			
		});			
	} 
	
	
	// methods
	$.extend(Scrollable.prototype, {  
			
			
		getVersion: function() {
			return '@VERSION';	
		},

		click: function(index) {
			
			var item = this.items.eq(index);
			var klass = this.opts.activeClass;			
			
			if (!item.hasClass(klass) && (index >= 0 || index < this.items.size())) {				
				this.items.removeClass(klass);
				item.addClass(klass);
				var delta = Math.floor(this.opts.size / 2);
				var to = index - delta;

				if (to !== this.activeIndex) {
					this.seekTo(to);		
				}				 
			} 
		},
		
		
		getStatus: function() {
			var len =  this.items.size();
			return {
				size: this.opts.size,
				total: len, 
				index: this.index,  
				pages: Math.ceil(len / this.opts.size),
				page: Math.ceil(this.index / this.opts.size)
			};
		}, 

		
		// all other seeking functions depend on this generic seeking function		
		seekTo: function(index, time) {
			
			if (index < 0) { index = 0; }
			var max = Math.min(index, this.items.length - this.opts.size);  			
			
			if (index <= max) { 
				
				var item = this.items.eq(index);			
				this.index = index;	

				if (this.opts.horizontal) {
					var left = this.wrap.offset().left - item.offset().left;				
					this.wrap.animate({left: left}, time || this.opts.speed);
					
				} else {
					var top = this.wrap.offset().top - item.offset().top;					
					this.wrap.animate({top: top}, time || this.opts.speed);							
				}
				
				Scrollable.current = this; 
			} 
			

			// custom onSeek callback
			if ($.isFunction(this.opts.onSeek)) {
				this.opts.onSeek.call(this);
			}
			
			// navi status update
			var navi = $(this.opts.navi, this.root);
			
			if (navi.length) {
				var klass = this.opts.activeClass;
				var page = Math.ceil(index / this.opts.size);
				page = Math.min(page, navi.children().length - 1);
				navi.children().removeClass(klass).eq(page).addClass(klass);
			} 
			
			this.activeIndex = index;			
			return true; 
		},
		
			
		move: function(offset, time) {
			var to = this.index + offset;
			if (this.opts.loop && to > (this.items.length - this.opts.size)) {
				to = 0;	
			}
			this.seekTo(to, time);
		},
		
		next: function(time) {
			this.move(1, time);	
		},
		
		prev: function(time) {
			this.move(-1, time);	
		},
		
		movePage: function(offset, time) {
			this.move(this.opts.size * offset, time);		
		},
		
		setPage: function(page, time) {
			var size = this.opts.size;
			var index = size * page;
 			var lastPage = index + size >= this.items.size(); 
			if (lastPage) {
				index = this.items.size() - this.opts.size;
			}
			this.seekTo(index, time);
		},
		
		prevPage: function(time) {
			this.setPage(this.getStatus().page - 1, time);
		},  

		nextPage: function(time) {
			this.setPage(this.getStatus().page + 1, time);
		}, 
		
		begin: function(time) {
			this.seekTo(0, time);	
		},
		
		end: function(time) {
			this.seekTo(this.items.size() - this.opts.size, time);	
		}
		
	});  
	
	
	
	// keyboard
	$(window).bind("keypress.scrollable", function(evt) {
		
		var el = Scrollable.current;	
		if (!el) { return; }
			
		if (el.opts.horizontal && (evt.keyCode == 37 || evt.keyCode == 39)) {
			el.move(evt.keyCode == 37 ? -1 : 1);
			return evt.preventDefault();
		}	
		
		if (!el.opts.horizontal && (evt.keyCode == 38 || evt.keyCode == 40)) {
			el.move(evt.keyCode == 38 ? -1 : 1);
			return evt.preventDefault();
		}
		
		return true;
		
	});	
		
	// jQuery plugin implementation
	jQuery.prototype.scrollable = function(opts, arg0, arg1) { 
			
		// return API associated with this instance
		if (!opts || typeof opts == 'number') {
			var index = opts || 0;
			var el = $.data(this.get()[index], "scrollable");
			if (el) { return el; }
		}
		
		this.each(function() {
				
			// @deprecated way of accessing API
			if (typeof opts == "string") {
				var el = $.data(this, "scrollable");
				el[opts].apply(el, [arg0, arg1]);
				
			// create new Scrollable instance
			} else { 
				var instance = new Scrollable(this, opts);	
				$.data(this, "scrollable", instance);
			}
		});
		
		return this;
	};
			
	
})(jQuery);



