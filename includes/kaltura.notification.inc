<?php
// $Id: kaltura.notification.inc,v 1.1.2.5.2.1 2010/11/17 01:14:57 xuriz Exp $

/*
 * @file
 * contains functions for handling all notifications from kaltura
 *
 */

/*
 * This is the calback function for the kaltura/notification_handler URL
 *
 * This function uses the KalturaNotificationClient class to normalize the received notifications
 * The mentioned class also validates the notification signature, to prevent malicious data injection
 *
 * Each notification is save in the notifications table, so in case of duplicate notification only
 * the first notificaion will be handled
 *
 * This function also invokes the hook_notification_handler so other modules that want to act upon
 * notification is received will be able to do so.
 */
function kaltura_notification_handler() {
  $admin_secret = variable_get('kaltura_admin_secret', '');
  //$params = $_REQUEST;
  $params = $_POST;
 /*   foreach($_POST as $key => $val)
  {
	error_log($key . "=>" . $val);
  }*/
  watchdog('kaltura NH', print_r($params, TRUE));
  unset($params['q']);
  $noti = new KalturaNotificationClient($params, $admin_secret);
  if ($noti->valid_signature === NULL) {
    watchdog('kaltura', 'notification params empty');
    return FALSE;
  }
  elseif ($noti->valid_signature === FALSE) {
    watchdog('kaltura', 'notification signature not valid');
    return FALSE;
  }
  else {
    watchdog('kaltura', 'hooray!!! notification signature valid');
  }
  echo 'OK';
  foreach ($noti->data as $notification_data) {
    $times = kaltura_notification_received($notification_data['notification_id']);
    if (!$times || $notification_data['notification_type'] == 'test') {
      kaltura_notification_save($notification_data['notification_id'], $notification_data);
      kaltura_forward_notification($notification_data);
      kaltura_invoke('notification_handler', $notification_data);
    }
    else {
      watchdog('notification', 'This notification ('. $notification_data['notification_id'] .') was already received');
    }
  }
}

/*
 * This function saves the received notification to the DB
 */
function kaltura_notification_save($notification_id, $notification_data) {
  $insert_query = 'INSERT INTO {kaltura_notifications} (notification_id, data, received_at) VALUES (%d, \'%s\', \''. date('Y-m-d H:i:s') .'\')';
  $result = db_query($insert_query, $notification_id, serialize($notification_data));
}

/*
 * This function checks if the received notification was already handled - to avoid duplicates
 */
function kaltura_notification_received($notification_id) {
  $query = 'SELECT COUNT(notification_id) c FROM {kaltura_notifications} WHERE notification_id = %d';
  $result = db_query($query, $notification_id);
  $count_obj = db_fetch_object($result);
  return $count_obj->c;
}

/*
 * This function check the notificatio type, and forwards the notification to the appropriate function
 *
 * Some notification types, as described in  kaltura API documentation, are not yet handled here
 */
function kaltura_forward_notification($noti) {
  watchdog('kaltura notification handler', print_r($noti, TRUE));
  switch ($noti['notification_type']) {
    case 'entry_add': 
	  if ($noti['media_type'] == 6)
	  {
		kaltura_notify_node_entry_add('mix', $noti);
	  }
	  else
	  {
		kaltura_notify_node_entry_add('entry', $noti);
	  }
      break;
    case 'kshow_add': kaltura_notify_node_entry_add('mix', $noti);
      break;
    case 'kshow_update_info': watchdog('KNH', 'notification of type kshow_update_info received<br />'. serialize($noti));
      break;
    case 'entry_update_thumbnail': kaltura_update_entry_thumbnail($noti);
      break;
    case 'entry_update': kaltura_notify_node_update($noti);
      break;
    case 'test': kaltura_test_notification_received();
      break;
    default:
  }
}

/*
 * helper function to set the notification_status variable to 1
 *
 * should only be called when test notification recieved
 */
function kaltura_test_notification_received() {
  variable_set('kaltura_notification_status', 1);
}

/*
 * helper function to update the thumbnail url of an entry
 */
function kaltura_update_entry_thumbnail($notification) {
  $thumburl = $notification['thumbnail_url'];
  $entry = $notification['entry_id'];
  $update = 'UPDATE {node_kaltura} SET kaltura_thumbnail_url = \'%s\' WHERE kaltura_entryId = \'%s\'';
  $result = db_query($update, $thumburl, $entry);
}

/*
 * function to handle with entry update notification
 */
function kaltura_notify_node_update($notification) {
  $get_nid_from_entry = 'SELECT nid FROM {node_kaltura} WHERE kaltura_entryId = \'%s\'';
  $nodes = db_query($get_nid_from_entry, $notification['entry_id']);
  while ($obj = db_fetch_object($nodes)) {
    $node_ids[] = $obj->nid;
  }

  $updatable_data = array('tags' => 'kaltura_tags', 'admin_tags' => 'kaltura_admin_tags', 'status' => 'kstatus','name' => 'title');
  if (count($node_ids) == 1) {
    $updated_node = node_load($node_ids[0]);
    $updated_node->notification_update = TRUE;
    $updated_node->body = kaltura_get_entry_description($notification['media_type'], $notification['entry_id']);
    foreach ($updatable_data as $notification_key => $node_key) {
      if ($updated_node->$node_key != $notification[$notification_key]) {
        $updated_node->$node_key = $notification[$notification_key];
      }
    }
    kaltura_push_extra_data($updated_node, $notification);
    node_save($updated_node);
  }
  else {
    //watchdog('nod', 'found more than 1 node '. count($node_ids));
  }
}

/*
 * function to handle both add entry notification and add kshow notification
 */
function kaltura_notify_node_entry_add($type, $notification_data) {
  if ($notification_data['partner_data']) {
    $data_exploded  = explode('|', $notification_data['partner_data']);
    foreach ($data_exploded as $data) {
      $splited = explode('@', $data);
      $partner_data[$splited[0]] = $splited[1];
    }
  }
  $node->title = $notification_data['name'];
//  $node->body = '';//'test '. time(); // print_r($notification_data,TRUE).PHP_EOL.print_r($_REQUEST,TRUE);
  $node->body = kaltura_get_entry_description($notification_data['media_type'], $notification_data['entry_id']);

  $node->type = 'kaltura_'. $type;
  $node->uid = ($notification_data['puser_id'])? $notification_data['puser_id']: FALSE;
  if ($node->uid === FALSE) {
    $node->uid = ($partner_data['user_id'])? $partner_data['user_id']: 0;
  }
  $node->status = 1;
  $node->created = time();
  $node->comment = variable_get('comment_kaltura_entry', 2);
  $node->changed = time();
  $node->promote = 0;
  $node->sticky = 0;
  $node->kstatus = $notification_data['status'];
  
  if ($partner_data['entry_as_comment'] == 'yes') {
    $node->kaltura_video_comment = 1;
  }
  else {
    $node->kaltura_video_comment = 0;
  }
  
  kaltura_push_extra_data($node, $notification_data);
  $node->kaltura_entry_data = serialize(array($notification_data));
  $node->kaltura_tags = $notification_data['tags'];
  if ($type == 'entry') {
    $node->kaltura_entryId = $notification_data['entry_id'];
  }
  else {
    if (WORK_WITH_KSHOW) {
      $node->kaltura_entryId = $notification_data['kshow_id'];
    }
    else {
      $node->kaltura_entryId = $notification_data['show_entry_id'];
    }
  }
  $node->kaltura_admin_tags = '';
  if (($type == 'mix' && $partner_data['create_kshow'] == 'yes') || $type == 'entry') {
    node_save($node);
    module_invoke('node_kaltura_'. $type, 'hook_insert', $node);
  }
}

/*
 * helper function to inject the node object with some extra details from the notification
 */
function kaltura_push_extra_data(&$node, $data) {
  $fields = array(
    'kaltura_media_type' => 'media_type',
    'kaltura_duration' => 'length_in_msecs',
    'kaltura_thumbnail_url' => 'thumbnail_url',
    'kaltura_partner_data' => 'partner_data',
    'kaltura_source' => 'source',
    'kaltura_source_id' => 'source_id',
    'kaltura_source_link' => 'source_link',
    'kaltura_width' => 'width',
    'kaltura_height' => 'height',
    'kaltura_download_url' => 'download_url',
    'kaltura_media_date' => 'media_date',
    'kaltura_views' => 'views',
    'kaltura_plays' => 'plays',
    'kaltura_votes' => 'votes',
    'kaltura_rank' => 'rank',
    'kaltura_total_rank' => 'total_rank',
  );
  foreach ($fields as $node_key => $field) {
    if ($data[$field]) {
      $node->$node_key = $data[$field];
    }
    if ($data[$field] && $field == 'length_in_msecs') {
      $node->$node_key = (int)($data[$field]/1000);
    }
  }
}

