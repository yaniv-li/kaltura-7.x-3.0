<?php
// $Id: kaltura_views_handler_field_kaltura_thumb.inc,v 1.1.2.1 2010/08/09 04:33:33 univate Exp $

/*
 * Declaration of new field handler that extends the basic field handler of views module
 * We want to theme the thumbnail field ourselvs.
 */
class kaltura_views_handler_field_kaltura_thumb extends views_handler_field {

  /*
   * return a themed HTML of a kaltura thumbnail field
   */
  function theme($data) {

    $node = new stdClass;
    $node->kaltura_thumbnail_url = $data->node_kaltura_kaltura_thumbnail_url;
    $node->nid = $data->nid;
    $node->link_thumb = TRUE;
    list( $kaltura_type, $kaltura_media_type, $kstatus ) = $this->kaltura_get_node_type($node->nid);
    $node->kaltura_media_type = $kaltura_media_type;
    $node->kstatus = $kstatus;
    $size = array();
    $size['width'] = $this->options['thumbnail_width'];
    $size['height'] = $this->options['thumbnail_height'];
    if ($kaltura_type == 'kaltura_mix') {
      return theme('node_kaltura_mix_thumbnail_url', $node, $teaser = TRUE, FALSE, $size);
    }
    else {
      return theme('node_kaltura_entry_thumbnail_url', $node, $teaser = TRUE, FALSE, $size);
    }
  }

  /*
   * helper function that gets some info about node with id $nid
   */
  function kaltura_get_node_type($nid) {
    $sql = 'SELECT type, kaltura_media_type, kstatus FROM {node} node LEFT JOIN {node_kaltura} knode ON node.nid = knode.nid WHERE node.nid = %d';
    $result = db_fetch_object(db_query($sql, $nid));
    return array( $result->type, $result->kaltura_media_type, $result->kstatus );
  }

  function has_extra_options() { return TRUE; }

  function option_definition() {
    $options = parent::option_definition();

    $options['thumbnail_width'] = array('default' => 100);
    $options['thumbnail_height'] = array('default' => 100);

    return $options;
  }

  function extra_options_form(&$form, &$form_state) {
    $form['thumbnail_width'] = array(
        '#type' => 'textfield',
        '#title' => t('Thumbnail Width'),
        '#size' => 5,
        '#default_value' => $this->options['thumbnail_width'],
    );
    $form['thumbnail_height'] = array(
        '#type' => 'textfield',
        '#title' => t('Thumbnail Height'),
        '#size' => 5,
        '#default_value' => $this->options['thumbnail_height'],
    );
    return $form;
  }
}


