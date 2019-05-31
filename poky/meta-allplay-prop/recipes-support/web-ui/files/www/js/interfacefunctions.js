/* vars */
var FADE_IN_TIME_MILLIS=500;
// array_of_images_to_preload can be found in the theme directory;
var array_of_images_to_preload = new Array();

/* functions */
window.onload = function(){
	/* handle keys */
	$(document).keydown(function(e) {

	});
	initialiseInterface();
}

		function initialiseInterface(){
			preloadImages();
			interface_functions_fade_in('wrapper_root');

			if ($('#connect_animation_overlay').length){
				connecting_animation();
			}

		}/* /initialiseInterface */

		function interface_functions_fade_in(fade_id){
			$('#'+fade_id).fadeIn(FADE_IN_TIME_MILLIS, function() { $('[tabindex=1]').focus(); });
		} /* /interface_functions_fade_in  */

		function preloadImages() {
			if (!preloadImages.list) {
				preloadImages.list = [];
			}
			for (var i = 0; i < array_of_images_to_preload.length; i++) {
				var img = new Image();
				img.src = array_of_images_to_preload[i];
				preloadImages.list.push(img);
			}
		} /* /preloadImages */

		function show_modal(setVisible){
			if ((!setVisible === 'undefined') || setVisible==true){
				if( $('#modal').is(':hidden') ) {
					$('#modal').fadeIn(FADE_IN_TIME_MILLIS);
					$('#wrapper_root').addClass( "modal_on" );
				}
			}
			else{
				$('#modal').fadeOut(FADE_IN_TIME_MILLIS);
				$('#wrapper_root').removeClass( "modal_on" );
			}
		}/* /show modal */

		/* connecting animation */
		function connecting_animation(){
			$('#connect_animation_overlay').fadeIn(1, function(){
				$('#connect_animation_overlay').fadeOut(1500, function(){
					connecting_animation();
				});
			});
		}
		/* /connecting animation */

function display_language_selections() {
	refresh_selected_language();
	if( $('#language_selection_dialog_wrapper').is(':hidden') ) {
		show_modal(true);
		$('#language_selection_dialog_wrapper').fadeIn(FADE_IN_TIME_MILLIS);
	}
	else{
		show_modal(false);
		$('#language_selection_dialog_wrapper').fadeOut(FADE_IN_TIME_MILLIS);
	}
} /* /display_language_selections */

function ie_radio_checkbox(){
	this.blur();
	this.focus();
}

