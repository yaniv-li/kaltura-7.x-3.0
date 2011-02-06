<?php
// $Id: kaltura_views.views.inc,v 1.1.2.2 2010/08/28 10:21:00 xuriz Exp $

/*
 * Implementation of hook_views_data().
 *
 * Provides the views module with data about node_kaltura table
 * Enables the user to add fields to a view and filter/sort by some fields.
 * Some of the fields have only a field handler, which means they could be added to the "fields" list of the view
 * Others have a filter handler so the view could be filtered by them
 *
 * The structure of the return value applies the views2 api
 */
function kaltura_views_data() {
  $media_id_help = '';
  if (module_exists('kaltura_playlist')) {
    $media_id_help = ' '. t('To have this view listed in Kaltura Playlist node, you must add this field to the default fields (can be excluded from display).');
  }
  $data = array();
  $data['node_kaltura'] = array(
    'table' => array(
      'group' => 'kaltura',
      'title' => 'kaltura',
      'help' => t('table that holds kaltura entries'),
      'base' => array(
        'field' => 'nid',
        'title' => 'node id',
      ),
      'join' => array(
        'node' => array(
          'left_field' => 'nid',
          'field' => 'nid',
        ),
      ),
    ),
    'kaltura_media_type' => array(
      'title' => t('kaltura entry media type'),
      'help' => t('see kaltura documentation for values'),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click_sortable' => FALSE,
      ),
      'filter' => array(
        'handler' => 'kaltura_views_handler_filter_media_type',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'kaltura_plays' => array(
      'title' => t('kaltura item number of plays'),
      'help' => t(''),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click_sortable' => FALSE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'kaltura_views' => array(
      'title' => t('kaltura item number of views'),
      'help' => t('see kaltura documentation for values'),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click_sortable' => FALSE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'kaltura_admin_tags' => array(
      'title' => t('kaltura item admin tags'),
      'help' => t(''),
      'field' => array(
        'handler' => 'views_handler_field',
        'click_sortable' => FALSE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'kaltura_tags' => array(
      'title' => t('kaltura item tags'),
      'help' => t(''),
      'field' => array(
        'handler' => 'views_handler_field',
        'click_sortable' => FALSE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'kaltura_duration' => array(
      'title' => t('kaltura item duration'),
      'help' => t(''),
      'field' => array(
        'handler' => 'kaltura_views_handler_field_kaltura_duration',
        'click_sortable' => FALSE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'kaltura_total_rank' => array(
      'title' => t('kaltura item total rank'),
      'help' => t('total rank is rank multiplied by 1000'),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click_sortable' => FALSE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'kaltura_votes' => array(
      'title' => t('kaltura item number of votes'),
      'help' => t('see kaltura documentation for values'),
      'field' => array(
        'handler' => 'kaltura_views_handler_field_kaltura_votes',
        'click_sortable' => FALSE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'kaltura_rank' => array(
      'title' => t('kaltura item rank'),
      'help' => t('see kaltura documentation for values'),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click_sortable' => FALSE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'kaltura_thumbnail_url' => array(
      'title' => t('kaltura item thumbnail'),
      'help' => t(''),
      'field' => array(
        'handler' => 'kaltura_views_handler_field_kaltura_thumb',
      ),
    ),
    'kaltura_source_link' => array(
      'title' => t('kaltura item source link'),
      'help' => t(''),
      'field' => array(
        'handler' => 'views_handler_field',
      ),
    ),
    'kaltura_download_url' => array(
      'title' => t('kaltura item download link'),
      'help' => t(''),
      'field' => array(
        'handler' => 'kaltura_views_handler_field_url',
      ),
    ),
    'kaltura_media_date' => array(
      'title' => t('kaltura image item media date (EXIF data)'),
      'help' => t(''),
      'field' => array(
        'handler' => 'views_handler_field',
      ),
    ),
    'kaltura_entryId' => array(
      'title' => t('kaltura media ID'),
      'help' => t('') . $media_id_help,
      'field' => array(
        'handler' => 'views_handler_field',
      ),
    ),
    'kaltura_video_comment' => array(
      'title' => t('was the Kaltura node created when adding a comment on another node'),
       'help' => t(''),
      'field' => array(
        'handler' => 'views_handler_field',
      ),
      'filter' => array(
        'handler' => 'kaltura_views_handler_filter_is_comment',
      ),
    ),
  );
  return $data;
}

/**
 * Implementation of hook_views_handlers().
 */
function kaltura_views_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'kaltura_views'),
    ),
    'handlers' => array(
      // fields
      'kaltura_views_handler_field_kaltura_thumb' => array( 'parent' => 'views_handler_field',),
      'kaltura_views_handler_field_kaltura_duration' => array( 'parent' => 'views_handler_field',),
      'kaltura_views_handler_field_kaltura_votes' => array( 'parent' => 'views_handler_field',),
      // fiters
      'kaltura_views_handler_filter_media_type' => array('parent' => 'views_handler_filter_in_operator',),
      'kaltura_views_handler_filter_is_comment' => array('parent' => 'views_handler_filter_in_operator',),
      // arguments
    ),
  );
}

