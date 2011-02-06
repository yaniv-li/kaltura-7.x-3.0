<?php
// $Id: kaltura_views_handler_field_kaltura_votes.inc,v 1.1.2.1 2010/08/09 04:33:33 univate Exp $

/**
 *
 */
class kaltura_views_handler_field_kaltura_votes extends views_handler_field {
  function theme($data) {
    $votes = $data->node_kaltura_kaltura_votes;
    return t('@votes counts', array('@votes' => $votes));
//    return theme('node_kaltura_entry_duration', $durationoutput);
  }
}
