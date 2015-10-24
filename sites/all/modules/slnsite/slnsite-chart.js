
/**
 * JavaScript behaviors for the front-end display of radar charts in project reports.
 */

(function ($) {

  Drupal.settings.slnsite = Drupal.settings.slnsite || {};
  //Drupal.behaviors.slnsite = Drupal.behaviors.slnsite || {};
  //Drupal.slnsite = Drupal.slnsite || {};

  // TODO: constrain in div with % size?
  //Chart.defaults.global.responsive = true;

  // Boolean - If we want to override with a hard coded scale
  Chart.defaults.global.scaleOverride = true;

  // ** Required if scaleOverride is true **
  // Number - The number of steps in a hard coded scale
  Chart.defaults.global.scaleSteps = 4;
  // Number - The value jump in the hard coded scale
  Chart.defaults.global.scaleStepWidth = 1;
  // Number - The scale starting value
  Chart.defaults.global.scaleStartValue = 1;


  var myRadar;

  $.getJSON( Drupal.settings.slnsite.dataurl , function( json ) {

    //console.log(json);
    
    myRadar = new Chart(document.getElementById("sln-radar-canvas").getContext("2d")).Radar(json, {scaleShowLabels : true, scaleBeginAtZero : true, pointDot : false, pointLabelFontSize : 16});
    var div_legend = document.getElementById('sln-radar-legend');
    div_legend.innerHTML = myRadar.generateLegend();
    
    //checkReady();
    setTimeout("checkReady()", 5000);
    
    /*
    $('#sln-radar-canvas').live('load', function() {
      var img_png = document.getElementById('sln-radar-img'); 
      //alert('Hey!');
      img_png.setAttribute("src", myRadar.toBase64Image());
      $('#sln-radar-canvas').hide();
    });
    */
    
  });


})(jQuery);


function checkReady() {
  /*var svg = document.getElementById("sln-radar-canvas");
  var svgdoc = svg.getSVGDocument();
  if (svgdoc == null) {
      setTimeout("checkReady()", 300);
  } else {*/
    //alert("hi");
    try {
      $('#sln-radar-img').show();
      var img_png = document.getElementById('sln-radar-img'); 
      //alert('Hey!');
      img_png.setAttribute("src", myRadar.toBase64Image());
      $('#sln-radar-canvas').hide();
    } catch (e) {
    
    }
 //}
}

