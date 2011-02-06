<?php
// $Id: kaltura_views.views_default.inc,v 1.1.2.1 2010/08/09 04:33:33 univate Exp $

/**
 * Implementation of hook_views_default_views().
 *
 * We provide the following default views, which the user can override
 * Recently uploaded videos
 * Recently uploaded photos
 * Most viewed items
 * Highest rated items
 * My Kaltura Items - shows the items of the current user
 */
function kaltura_views_default_views() {
  $view = new view;
  $view->name = 'kaltura_most_popular_videos';
  $view->description = 'Displays the thumbnails of the videos that were most played. '.
    'Placing the mouse over the thumbnail will create a quick preview of '.
    'remixes and collaborative videos. Clicking the thumbnail will lead to the video node page where the video can be viewed.';
  $view->tag = 'kaltura';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Most Popular Videos', 'default');
  $handler->override_option('fields', array(
    'kaltura_thumbnail_url' => array(
      'label' => '',
      'exclude' => 0,
      'id' => 'kaltura_thumbnail_url',
      'table' => 'node_kaltura',
      'field' => 'kaltura_thumbnail_url',
      'relationship' => 'none',
    ),
    'title' => array(
      'label' => '',
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none',
    ),
    'kaltura_entryId' => array(
      'label' => '',
      'exclude' => 1,
      'id' => 'kaltura_entryId',
      'table' => 'node_kaltura',
      'field' => 'kaltura_entryId',
      'relationship' => 'none',
    ),
    'kaltura_plays' => array(
      'label' => 'played',
      'set_precision' => FALSE,
      'precision' => 0,
      'decimal' => '.',
      'separator' => ',',
      'prefix' => '',
      'suffix' => '',
      'exclude' => 0,
      'id' => 'kaltura_plays',
      'table' => 'node_kaltura',
      'field' => 'kaltura_plays',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
      'timestamp' => array(
      'label' => 'Created at',
      'prefix' => '',
      'suffix' => '',
      'exclude' => 0,
      'date_format' => 'small',
       'field' => 'timestamp',
       'id' => 'timestamp',
      'table' => 'node_revisions',
      'relationship' => 'none'
     ),
    'kaltura_duration' => array(
      'label' => '',
      'set_precision' => FALSE,
      'precision' => 0,
      'decimal' => '.',
      'separator' => ',',
      'prefix' => '',
      'suffix' => '',
      'exclude' => 0,
      'id' => 'kaltura_duration',
      'table' => 'node_kaltura',
      'field' => 'kaltura_duration',
      'relationship' => 'none',
    ),
    'kaltura_tags' => array(
      'label' => 'Tags',
      'exclude' => 0,
      'id' => 'kaltura_tags',
      'table' => 'node_kaltura',
      'field' => 'kaltura_tags',
      'relationship' => 'none',
      ),
    'kaltura_admin_tags' => array(
      'label' => 'Categories',
      'exclude' => 0,
      'id' => 'kaltura_admin_tags',
      'table' => 'node_kaltura',
      'field' => 'kaltura_admin_tags',
      'relationship' => 'none',
      ),
    'user_id' => array(
      'label' => '',
      'exclude' => 1,
      'id' => 'users_uid',
      'table' => 'users',
      'field' => 'uid',
      'relationship' => 'none',
      ),
    'user_name' => array(
      'label' => 'Created By',
      'exclude' => 0,
      'id' => 'users_name',
      'table' => 'users',
      'field' => 'name',
      'relationship' => 'none',
      ),
  ));
  $handler->override_option('sorts', array(
    'kaltura_plays' => array(
      'order' => 'DESC',
      'id' => 'kaltura_plays',
      'table' => 'node_kaltura',
      'field' => 'kaltura_plays',
    ),
  ));
  $handler->override_option('filters', array(
    'kaltura_media_type' => array(
      'operator' => 'in',
      'value' => array(
        '1' => '1',
        '6' => '6',
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => 'media_type_op',
        'label' => 'Filter by media type',
        'use_operator' => 1,
        'identifier' => 'kaltura_media_type',
        'optional' => 1,
        'single' => 0,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'kaltura_media_type',
      'table' => 'node_kaltura',
      'field' => 'kaltura_media_type',
      'relationship' => 'none',
    ),
    'kaltura_video_comment' => array(
      'operator' => 'in',
      'value' => array(
        '0' => '0',
      ),
      'exposed' => FALSE,
      'expose' => array(
        'operator' => 'is_comment_op',
        'label' => 'Filter by created as comment',
        'use_operator' => 1,
        'identifier' => 'kaltura_video_comment',
        'optional' => 1,
        'single' => 0,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'kaltura_video_comment',
      'table' => 'node_kaltura',
      'field' => 'kaltura_video_comment',
      'relationship' => 'none',
    ),
    'status' => array(
      'operator' => '=',
      'value' => 1,
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'status',
      'table' => 'node',
      'field' => 'status',
      'relationship' => 'none',
    ),
    'kaltura_tags' => array(
      'operator' => 'contains',
      'value' => '',
      'exposed' => TRUE,
      'expose' => array(
        'operator' => 'kaltura_tags_op',
        'label' => 'kaltura: kaltura item tags',
        'use_operator' => 1,
        'identifier' => 'kaltura_tags',
        'optional' => 1,
        'single' => 0,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'kaltura_tags',
      'table' => 'node_kaltura',
      'field' => 'kaltura_tags',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Most Popular Videos');
  $handler->override_option('empty', 'You did not create any Kaltura items');
  $handler->override_option('empty_format', '1');
  $handler->override_option('use_pager', '1');
  $handler->override_option('distinct', 1);
  $handler->override_option('style_plugin', 'list');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'type' => 'ul',
  ));
  $handler = $view->new_display('page', 'Most Popular Videos', 'page_1');
  $handler->override_option('path', 'kaltura/most/popular-videos');
  $handler->override_option('menu', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
    'name' => 'navigation',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
  ));
  $views[$view->name] = $view;
/***********************/
  $view = new view;
  $view->name = 'kaltura_most_popular_videos_all_fields';
  $view->description = 'Displays the thumbnails of the videos that were most played. '.
    'Placing the mouse over the thumbnail will create a quick preview of '.
    'remixes and collaborative videos. Clicking the thumbnail will lead to the video node page where the video can be viewed.';
  $view->tag = 'kaltura';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Most Popular Videos with All Fields', 'default');
  $handler->override_option('fields', array(
    'kaltura_thumbnail_url' => array(
      'label' => '',
      'exclude' => 0,
      'id' => 'kaltura_thumbnail_url',
      'table' => 'node_kaltura',
      'field' => 'kaltura_thumbnail_url',
      'relationship' => 'none',
    ),
    'title' => array(
      'label' => '',
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none',
    ),
    'kaltura_duration' => array(
      'label' => '',
      'set_precision' => FALSE,
      'precision' => 0,
      'decimal' => '.',
      'separator' => ',',
      'prefix' => '',
      'suffix' => '',
      'exclude' => 0,
      'id' => 'kaltura_duration',
      'table' => 'node_kaltura',
      'field' => 'kaltura_duration',
      'relationship' => 'none',
    ),
    'kaltura_entryId' => array(
      'label' => '',
      'exclude' => 1,
      'id' => 'kaltura_entryId',
      'table' => 'node_kaltura',
      'field' => 'kaltura_entryId',
      'relationship' => 'none',
    ),
    'kaltura_plays' => array(
      'label' => 'played',
      'set_precision' => FALSE,
      'precision' => 0,
      'decimal' => '.',
      'separator' => ',',
      'prefix' => '',
      'suffix' => '',
      'exclude' => 0,
      'id' => 'kaltura_plays',
      'table' => 'node_kaltura',
      'field' => 'kaltura_plays',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
      'timestamp' => array(
      'label' => 'Created at',
      'prefix' => '',
      'suffix' => '',
      'exclude' => 0,
      'date_format' => 'small',
       'field' => 'timestamp',
       'id' => 'timestamp',
      'table' => 'node_revisions',
      'relationship' => 'none'
     ),
    'kaltura_tags' => array(
      'label' => 'Tags',
      'exclude' => 0,
      'id' => 'kaltura_tags',
      'table' => 'node_kaltura',
      'field' => 'kaltura_tags',
      'relationship' => 'none',
      ),
    'kaltura_admin_tags' => array(
      'label' => 'Categories',
      'exclude' => 0,
      'id' => 'kaltura_admin_tags',
      'table' => 'node_kaltura',
      'field' => 'kaltura_admin_tags',
      'relationship' => 'none',
      ),
    'user_id' => array(
      'label' => '',
      'exclude' => 1,
      'id' => 'users_uid',
      'table' => 'users',
      'field' => 'uid',
      'relationship' => 'none',
      ),
    'user_name' => array(
      'label' => 'Created By',
      'exclude' => 0,
      'id' => 'users_name',
      'table' => 'users',
      'field' => 'name',
      'relationship' => 'none',
      ),
    'kaltura_plays' => array(
      'label' => 'Plays',
      'exclude' => 0,
      'id' => 'kaltura_plays',
      'table' => 'node_kaltura',
      'field' => 'kaltura_plays',
      'relationship' => 'none',
      ),
    'kaltura_rank' => array(
      'label' => '',
      'exclude' => 0,
      'id' => 'kaltura_rank',
      'table' => 'node_kaltura',
      'field' => 'kaltura_rank',
      'relationship' => 'none',
      ),
    'kaltura_votes' => array(
      'label' => 'Rank',
      'exclude' => 0,
      'id' => 'kaltura_votes',
      'table' => 'node_kaltura',
      'field' => 'kaltura_votes',
      'relationship' => 'none',
      ),      ));
  $handler->override_option('sorts', array(
    'kaltura_plays' => array(
      'order' => 'DESC',
      'id' => 'kaltura_plays',
      'table' => 'node_kaltura',
      'field' => 'kaltura_plays',
    ),
  ));
  $handler->override_option('filters', array(
    'kaltura_media_type' => array(
      'operator' => 'in',
      'value' => array(
        '1' => '1',
        '6' => '6',
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => 'media_type_op',
        'label' => 'Filter by media type',
        'use_operator' => 1,
        'identifier' => 'kaltura_media_type',
        'optional' => 1,
        'single' => 0,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'kaltura_media_type',
      'table' => 'node_kaltura',
      'field' => 'kaltura_media_type',
      'relationship' => 'none',
    ),
    'kaltura_video_comment' => array(
      'operator' => 'in',
      'value' => array(
        '0' => '0',
      ),
      'exposed' => FALSE,
      'expose' => array(
        'operator' => 'is_comment_op',
        'label' => 'Filter by created as comment',
        'use_operator' => 1,
        'identifier' => 'kaltura_video_comment',
        'optional' => 1,
        'single' => 0,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'kaltura_video_comment',
      'table' => 'node_kaltura',
      'field' => 'kaltura_video_comment',
      'relationship' => 'none',
    ),
    'status' => array(
      'operator' => '=',
      'value' => 1,
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'status',
      'table' => 'node',
      'field' => 'status',
      'relationship' => 'none',
    ),
    'kaltura_tags' => array(
      'operator' => 'contains',
      'value' => '',
      'exposed' => TRUE,
      'expose' => array(
        'operator' => 'kaltura_tags_op',
        'label' => 'kaltura: kaltura item tags',
        'use_operator' => 1,
        'identifier' => 'kaltura_tags',
        'optional' => 1,
        'single' => 0,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'kaltura_tags',
      'table' => 'node_kaltura',
      'field' => 'kaltura_tags',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Most Popular Videos with All Fields');
  $handler->override_option('empty', 'You did not create any Kaltura items');
  $handler->override_option('empty_format', '1');
  $handler->override_option('use_pager', '1');
  $handler->override_option('distinct', 1);
  $handler->override_option('style_plugin', 'list');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'type' => 'ul',
  ));
  $handler = $view->new_display('page', 'Most Popular Videos', 'page_1');
  $handler->override_option('path', 'kaltura/most/popular-videos-all-fields');
  $handler->override_option('menu', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
    'name' => 'navigation',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
  ));
  $views[$view->name] = $view;
/***********************/

  $view = new view;
  $view->name = 'kaltura_most_popular_audio';
  $view->description = 'Displays a list of the audio entries that were most listened to. Each name is a link to the audio node page.';
  $view->tag = 'kaltura';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Most Popular Audio', 'default');
  $handler->override_option('fields', array(
    'title' => array(
      'label' => '',
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none',
    ),
      'timestamp' => array(
      'label' => 'Created at',
      'prefix' => '',
      'suffix' => '',
      'exclude' => 0,
      'date_format' => 'small',
       'field' => 'timestamp',
       'id' => 'timestamp',
      'table' => 'node_revisions',
      'relationship' => 'none'
     ),
    'kaltura_duration' => array(
      'label' => '',
      'set_precision' => FALSE,
      'precision' => 0,
      'decimal' => '.',
      'separator' => ',',
      'prefix' => '',
      'suffix' => '',
      'exclude' => 0,
      'id' => 'kaltura_duration',
      'table' => 'node_kaltura',
      'field' => 'kaltura_duration',
      'relationship' => 'none',
    ),
    'kaltura_tags' => array(
      'label' => 'Tags',
      'exclude' => 0,
      'id' => 'kaltura_tags',
      'table' => 'node_kaltura',
      'field' => 'kaltura_tags',
      'relationship' => 'none',
      ),
    'kaltura_admin_tags' => array(
      'label' => 'Categories',
      'exclude' => 0,
      'id' => 'kaltura_admin_tags',
      'table' => 'node_kaltura',
      'field' => 'kaltura_admin_tags',
      'relationship' => 'none',
      ),
    'user_id' => array(
      'label' => '',
      'exclude' => 1,
      'id' => 'users_uid',
      'table' => 'users',
      'field' => 'uid',
      'relationship' => 'none',
      ),
    'user_name' => array(
      'label' => 'Created By',
      'exclude' => 0,
      'id' => 'users_name',
      'table' => 'users',
      'field' => 'name',
      'relationship' => 'none',
      ),
  ));
  $handler->override_option('sorts', array(
    'kaltura_plays' => array(
      'order' => 'DESC',
      'id' => 'kaltura_plays',
      'table' => 'node_kaltura',
      'field' => 'kaltura_plays',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'kaltura_media_type' => array(
      'operator' => 'in',
      'value' => array(
        '5' => '5',
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => 'media_type_op',
        'label' => 'Filter by media type',
        'use_operator' => 1,
        'identifier' => 'kaltura_media_type',
        'optional' => 1,
        'single' => 0,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'kaltura_media_type',
      'table' => 'node_kaltura',
      'field' => 'kaltura_media_type',
      'relationship' => 'none',
    ),
    'kaltura_video_comment' => array(
      'operator' => 'in',
      'value' => array(
        '0' => '0',
      ),
      'exposed' => FALSE,
      'expose' => array(
        'operator' => 'is_comment_op',
        'label' => 'Filter by created as comment',
        'use_operator' => 1,
        'identifier' => 'kaltura_video_comment',
        'optional' => 1,
        'single' => 0,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'kaltura_video_comment',
      'table' => 'node_kaltura',
      'field' => 'kaltura_video_comment',
      'relationship' => 'none',
    ),
    'status' => array(
      'operator' => '=',
      'value' => 1,
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'status',
      'table' => 'node',
      'field' => 'status',
      'relationship' => 'none',
    ),
    'kaltura_tags' => array(
      'operator' => 'contains',
      'value' => '',
      'exposed' => TRUE,
      'expose' => array(
        'operator' => 'kaltura_tags_op',
        'label' => 'kaltura: kaltura item tags',
        'use_operator' => 1,
        'identifier' => 'kaltura_tags',
        'optional' => 1,
        'single' => 0,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'kaltura_tags',
      'table' => 'node_kaltura',
      'field' => 'kaltura_tags',
      'relationship' => 'none',
    ),

  ));
  $handler->override_option('access', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Most Popular Audio');
  $handler->override_option('empty', 'You did not create any Kaltura items');
  $handler->override_option('empty_format', '1');
  $handler->override_option('use_pager', '1');
  $handler->override_option('distinct', 1);
  $handler->override_option('style_plugin', 'list');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'type' => 'ul',
  ));
  $handler = $view->new_display('page', 'Most Popular Audio', 'page_1');
  $handler->override_option('path', 'kaltura/most/popular-audio');
  $handler->override_option('menu', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
    'name' => 'navigation',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
  ));
  $views[$view->name] = $view;

  $view = new view;
  $view->name = 'kaltura_recently_added_videos';
  $view->description = 'kaltura_recently_added_videos';
  $view->tag = 'kaltura';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Most Recently Uploaded Videos', 'default');

  $handler->override_option('fields', array(
    'kaltura_thumbnail_url' => array(
      'label' => '',
      'exclude' => 0,
      'id' => 'kaltura_thumbnail_url',
      'table' => 'node_kaltura',
      'field' => 'kaltura_thumbnail_url',
      'relationship' => 'none',
    ),
    'title' => array(
      'label' => '',
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none',
    ),
    'kaltura_duration' => array(
      'label' => 'kaltura item duration',
      'set_precision' => FALSE,
      'precision' => 0,
      'decimal' => '.',
      'separator' => ',',
      'prefix' => '',
      'suffix' => '',
      'exclude' => 0,
      'id' => 'kaltura_duration',
      'table' => 'node_kaltura',
      'field' => 'kaltura_duration',
      'relationship' => 'none',
    ),
    'kaltura_entryId' => array(
      'label' => '',
      'exclude' => 1,
      'id' => 'kaltura_entryId',
      'table' => 'node_kaltura',
      'field' => 'kaltura_entryId',
      'relationship' => 'none',
    ),
      'timestamp' => array(
      'label' => 'Created at',
      'prefix' => '',
      'suffix' => '',
      'exclude' => 0,
      'date_format' => 'small',
       'field' => 'timestamp',
       'id' => 'timestamp',
      'table' => 'node_revisions',
      'relationship' => 'none'
     ),
    'kaltura_duration' => array(
      'label' => '',
      'set_precision' => FALSE,
      'precision' => 0,
      'decimal' => '.',
      'separator' => ',',
      'prefix' => '',
      'suffix' => '',
      'exclude' => 0,
      'id' => 'kaltura_duration',
      'table' => 'node_kaltura',
      'field' => 'kaltura_duration',
      'relationship' => 'none',
    ),
    'kaltura_tags' => array(
      'label' => 'Tags',
      'exclude' => 0,
      'id' => 'kaltura_tags',
      'table' => 'node_kaltura',
      'field' => 'kaltura_tags',
      'relationship' => 'none',
      ),
    'kaltura_admin_tags' => array(
      'label' => 'Categories',
      'exclude' => 0,
      'id' => 'kaltura_admin_tags',
      'table' => 'node_kaltura',
      'field' => 'kaltura_admin_tags',
      'relationship' => 'none',
      ),
    'user_id' => array(
      'label' => '',
      'exclude' => 1,
      'id' => 'users_uid',
      'table' => 'users',
      'field' => 'uid',
      'relationship' => 'none',
      ),
    'user_name' => array(
      'label' => 'Created By',
      'exclude' => 0,
      'id' => 'users_name',
      'table' => 'users',
      'field' => 'name',
      'relationship' => 'none',
      ),
  ));
  $handler->override_option('sorts', array(
    'created' => array(
      'order' => 'DESC',
      'granularity' => 'second',
      'id' => 'created',
      'table' => 'node',
      'field' => 'created',
      'relationship' => 'none',
    ),
    'sticky' => array(
      'order' => 'DESC',
      'id' => 'sticky',
      'table' => 'node',
      'field' => 'sticky',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'type' => array(
      'operator' => 'in',
      'value' => array(
        'kaltura_entry' => 'kaltura_entry',
        'kaltura_mix' => 'kaltura_mix',
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'type',
      'table' => 'node',
      'field' => 'type',
      'relationship' => 'none',
    ),
    'status' => array(
      'operator' => '=',
      'value' => 1,
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'status',
      'table' => 'node',
      'field' => 'status',
      'relationship' => 'none',
    ),
    'kaltura_media_type' => array(
      'operator' => 'in',
      'value' => array(
        '1' => '1',
        '6' => '6',
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'kaltura_media_type',
      'table' => 'node_kaltura',
      'field' => 'kaltura_media_type',
      'relationship' => 'none',
      'override' => array(
        'button' => 'Override',
      ),
    ),
    'kaltura_video_comment' => array(
      'operator' => 'in',
      'value' => array(
        '0' => '0',
      ),
      'exposed' => FALSE,
      'expose' => array(
        'operator' => 'is_comment_op',
        'label' => 'Filter by created as comment',
        'use_operator' => 1,
        'identifier' => 'kaltura_video_comment',
        'optional' => 1,
        'single' => 0,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'kaltura_video_comment',
      'table' => 'node_kaltura',
      'field' => 'kaltura_video_comment',
      'relationship' => 'none',
    ),
    'kaltura_tags' => array(
      'operator' => 'contains',
      'value' => '',
      'exposed' => TRUE,
      'expose' => array(
        'operator' => 'kaltura_tags_op',
        'label' => 'kaltura: kaltura item tags',
        'use_operator' => 1,
        'identifier' => 'kaltura_tags',
        'optional' => 1,
        'single' => 0,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'kaltura_tags',
      'table' => 'node_kaltura',
      'field' => 'kaltura_tags',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Most Recently Uploaded Videos');
  $handler->override_option('distinct', 1);
  $handler->override_option('style_plugin', 'list');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'type' => 'ul',
  ));
  $handler = $view->new_display('page', 'Most Recently Uploaded Videos', 'page_1');
  $handler->override_option('path', 'kaltura/recently/videos');
  $handler->override_option('menu', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
    'name' => 'navigation',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
  ));
  $handler = $view->new_display('block', 'Block', 'block_1');
  $handler->override_option('block_description', '');
  $handler->override_option('block_caching', -1);
  $views[$view->name] = $view;

  $view = new view;
  $view->name = 'kaltura_my_media';
  $view->description = 'logged-in user\'s media';
  $view->tag = 'kaltura';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'My Kaltura Items', 'default');

  $handler->override_option('fields', array(
    'kaltura_thumbnail_url' => array(
      'label' => '',
      'exclude' => 0,
      'id' => 'kaltura_thumbnail_url',
      'table' => 'node_kaltura',
      'field' => 'kaltura_thumbnail_url',
      'relationship' => 'none',
    ),
    'title' => array(
      'label' => '',
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none',
    ),
    'kaltura_entryId' => array(
      'label' => '',
      'exclude' => 1,
      'id' => 'kaltura_entryId',
      'table' => 'node_kaltura',
      'field' => 'kaltura_entryId',
      'relationship' => 'none',
    ),
      'timestamp' => array(
      'label' => 'Created at',
      'prefix' => '',
      'suffix' => '',
      'exclude' => 0,
      'date_format' => 'small',
       'field' => 'timestamp',
       'id' => 'timestamp',
      'table' => 'node_revisions',
      'relationship' => 'none'
     ),
    'kaltura_duration' => array(
      'label' => '',
      'set_precision' => FALSE,
      'precision' => 0,
      'decimal' => '.',
      'separator' => ',',
      'prefix' => '',
      'suffix' => '',
      'exclude' => 0,
      'id' => 'kaltura_duration',
      'table' => 'node_kaltura',
      'field' => 'kaltura_duration',
      'relationship' => 'none',
    ),
    'kaltura_tags' => array(
      'label' => 'Tags',
      'exclude' => 0,
      'id' => 'kaltura_tags',
      'table' => 'node_kaltura',
      'field' => 'kaltura_tags',
      'relationship' => 'none',
      ),
    'kaltura_admin_tags' => array(
      'label' => 'Categories',
      'exclude' => 0,
      'id' => 'kaltura_admin_tags',
      'table' => 'node_kaltura',
      'field' => 'kaltura_admin_tags',
      'relationship' => 'none',
      ),
    'user_id' => array(
      'label' => '',
      'exclude' => 1,
      'id' => 'users_uid',
      'table' => 'users',
      'field' => 'uid',
      'relationship' => 'none',
      ),
    'user_name' => array(
      'label' => 'Created By',
      'exclude' => 0,
      'id' => 'users_name',
      'table' => 'users',
      'field' => 'name',
      'relationship' => 'none',
      ),

  ));
  $handler->override_option('filters', array(
    'uid_current' => array(
      'operator' => '=',
      'value' => 1,
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'uid_current',
      'table' => 'users',
      'field' => 'uid_current',
      'relationship' => 'none',
    ),
    'kaltura_media_type' => array(
      'operator' => 'in',
      'value' => array(
        '1' => '1',
        '2' => '2',
        '5' => '5',
        '6' => '6',
      ),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 1,
        'operator' => 'media_type_op',
        'identifier' => 'kaltura_media_type',
        'label' => 'Filter by media type',
        'optional' => 1,
        'single' => 0,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'kaltura_media_type',
      'table' => 'node_kaltura',
      'field' => 'kaltura_media_type',
      'relationship' => 'none',
    ),
    'kaltura_video_comment' => array(
      'operator' => 'in',
      'value' => array(
        '0' => '0',
      ),
      'exposed' => FALSE,
      'expose' => array(
        'operator' => 'is_comment_op',
        'label' => 'Filter by created as comment',
        'use_operator' => 1,
        'identifier' => 'kaltura_video_comment',
        'optional' => 1,
        'single' => 0,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'kaltura_video_comment',
      'table' => 'node_kaltura',
      'field' => 'kaltura_video_comment',
      'relationship' => 'none',
    ),
    'kaltura_tags' => array(
      'operator' => 'contains',
      'value' => '',
      'exposed' => TRUE,
      'expose' => array(
        'operator' => 'kaltura_tags_op',
        'label' => 'kaltura: kaltura item tags',
        'use_operator' => 1,
        'identifier' => 'kaltura_tags',
        'optional' => 1,
        'single' => 0,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'kaltura_tags',
      'table' => 'node_kaltura',
      'field' => 'kaltura_tags',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'My Kaltura Items');
  $handler->override_option('empty', 'You did not create any Kaltura items');
  $handler->override_option('empty_format', '1');
  $handler->override_option('use_pager', '1');
  $handler->override_option('distinct', 1);
  $handler->override_option('style_plugin', 'list');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'type' => 'ul',
  ));
  $handler = $view->new_display('page', 'My Kaltura Items', 'page_1');
  $handler->override_option('path', 'kaltura/my_items');
  $handler->override_option('menu', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
    'name' => 'navigation',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
  ));
  $views[$view->name] = $view;

  $view = new view;
  $view->name = 'kaltura_recently_added_photos';
  $view->description = 'kaltura recently added photos';
  $view->tag = 'kaltura';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Most Recently Uploaded Photos', 'default');
  $handler->override_option('fields', array(
    'kaltura_thumbnail_url' => array(
      'label' => '',
      'exclude' => 0,
      'id' => 'kaltura_thumbnail_url',
      'table' => 'node_kaltura',
      'field' => 'kaltura_thumbnail_url',
      'relationship' => 'none',
    ),
    'title' => array(
      'label' => '',
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none',
    ),
    'name' => array(
      'label' => '',
      'link_to_user' => 0,
      'exclude' => 0,
      'id' => 'name',
      'table' => 'users',
      'field' => 'name',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('sorts', array(
    'created' => array(
      'order' => 'DESC',
      'granularity' => 'second',
      'id' => 'created',
      'table' => 'node',
      'field' => 'created',
      'relationship' => 'none',
    ),
    'sticky' => array(
      'order' => 'DESC',
      'id' => 'sticky',
      'table' => 'node',
      'field' => 'sticky',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'type' => array(
      'operator' => 'in',
      'value' => array(
        'kaltura_entry' => 'kaltura_entry',
        'kaltura_mix' => 'kaltura_mix',
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'type',
      'table' => 'node',
      'field' => 'type',
      'relationship' => 'none',
    ),
    'status' => array(
      'operator' => '=',
      'value' => 1,
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'status',
      'table' => 'node',
      'field' => 'status',
      'relationship' => 'none',
    ),
    'kaltura_media_type' => array(
      'operator' => 'in',
      'value' => array(
        '2' => '2',
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'kaltura_media_type',
      'table' => 'node_kaltura',
      'field' => 'kaltura_media_type',
      'relationship' => 'none',
      'override' => array(
        'button' => 'Override',
      ),
    ),
    'kaltura_video_comment' => array(
      'operator' => 'in',
      'value' => array(
        '0' => '0',
      ),
      'exposed' => FALSE,
      'expose' => array(
        'operator' => 'is_comment_op',
        'label' => 'Filter by created as comment',
        'use_operator' => 1,
        'identifier' => 'kaltura_video_comment',
        'optional' => 1,
        'single' => 0,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'kaltura_video_comment',
      'table' => 'node_kaltura',
      'field' => 'kaltura_video_comment',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Most Recently Uploaded Photos');
  $handler->override_option('use_ajax', TRUE);
  $handler->override_option('use_pager', '1');
  $handler->override_option('distinct', 1);
  $handler->override_option('style_plugin', 'list');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'type' => 'ul',
  ));
  $handler = $view->new_display('page', 'Most Recently Uploaded Photos', 'page_1');
  $handler->override_option('path', 'kaltura/recently/photos');
  $handler->override_option('menu', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
    'name' => 'navigation',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
  ));
  $handler = $view->new_display('block', 'Block', 'block_1');
  $handler->override_option('block_description', '');
  $handler->override_option('block_caching', -1);
  $views[$view->name] = $view;

  return $views;
}
