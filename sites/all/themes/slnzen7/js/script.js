/**
 * @file
 * A JavaScript file for the theme.
 *
 * In order for this JavaScript to be loaded on pages, see the instructions in
 * the README.txt next to this file.
 */

// JavaScript should be made compatible with libraries other than jQuery by
// wrapping it with an "anonymous closure". See:
// - https://drupal.org/node/1446420
// - http://www.adequatelygood.com/2010/3/JavaScript-Module-Pattern-In-Depth
(function ($, Drupal, window, document, undefined) {


// To understand behaviors, see https://drupal.org/node/756722#behaviors
Drupal.behaviors.my_custom_behavior = {
  attach: function(context, settings) {

    // Place your code here.
    /*
    // mss: attempt to jump to page by clicking on progress bar numbers
    // this won't work due to webform page processing logic
    // idea: use conditional rules?
    $('.webform-progressbar-page').on('click', function() {
      var page_num = $('input[name="details[page_num]"]').val();
      var target_num = $( this ).find('.webform-progressbar-page-number').text();
      //alert(page_num);
      $('input[name="details[page_num]"]').val(target_num);
      alert( $('input[name="details[page_num]"]').val() );
      if (page_num < target_num) 
        $('.webform-next').click();
      if (page_num > target_num)
        $('.webform-previous').click();
      
    });
    */
    
    $('.role-administrator.sln-survey-form .webform-progressbar, .role-administrator.page-survey .webform-progressbar').append($('.form-actions').html());
    
  
    $('.webform-layout-box .form-textarea').attr('rows', 2);
  }
};


})(jQuery, Drupal, this, this.document);


