<?php
// $Id: kaltura_views_handler_field_kaltura_duration.inc,v 1.1.2.1 2010/08/09 04:33:33 univate Exp $

/**
 * Declaration of new field handler that extends the basic field handler of views module
 * We want to theme the duration field ourselvs.
 */
class kaltura_views_handler_field_kaltura_duration extends views_handler_field {
  function theme($data) {
    $secs = $data->node_kaltura_kaltura_duration;
      if ($secs > 60*60) { $hr = (int)($secs/60); $hr = ($hr < 10)? '0'. $hr: $hr; $min = (int)($secs/60/60); $min = ($min < 10)? '0'. $min: $min; $sec = (int)(($secs/60)%60); $sec = ($sec < 10)? '0'. $sec: $sec; $durationoutput = ($hr .':'. ($min) .':'. ($sec)); }
      else { $min = (int)($secs/60); $min = ($min < 10)? '0'. $min: $min; $sec = (int)($secs%60); $sec = ($sec < 10)? '0'. $sec: $sec; $durationoutput = ($min .':'. ($sec)); }
//  return $durationoutput;
    return theme('node_kaltura_entry_duration', $durationoutput);
  }
}


