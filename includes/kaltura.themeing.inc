<?php
// $Id: kaltura.themeing.inc,v 1.1.2.5.2.3 2011/01/06 10:40:44 xuriz Exp $

/*
 * @file
 * defines all themeing functions for kaltura core module
 *
 */
$GLOBALS['_kaltura_client_folder'] = drupal_get_path('module', 'kaltura') .'/kaltura_client/';

/*
 * helper function that is called in nodeAPI:alter
 *
 * this function gets the content to be displayed,
 * and returns the content with an embed tag instead of a "kaltura tag"
 *
 * some of the return content is a javascript with handler functions for the buttons on the player
 */
function kaltura_replace_tags($content, $is_comment = FALSE, $replace_to_thumb = FALSE, $edit_mix_access = FALSE, $show_embed = FALSE, $content_type = null) {

  global $user, $multibyte;
  $kaltura_editor_width = (variable_get('kaltura_editor_width', ''))? variable_get('kaltura_editor_width', ''): KalturaSettings_SE_WIDTH;
  $kaltura_editor_height = (variable_get('kaltura_editor_height', ''))? variable_get('kaltura_editor_height', ''): KalturaSettings_SE_HEIGHT;
  $kaltura_editor_URL = (variable_get('kaltura_editor_URL', ''))? variable_get('kaltura_editor_URL', ''): KalturaSettings_SE_URL;
  $added_playlist = false;
  
  
  
  $length = drupal_strlen($content);
  // add PHP_EOL before each kaltura widget to ensure correct grep_match results
  $content = str_replace('[kaltura-widget', PHP_EOL .'[kaltura-widget', $content);
  $found = FALSE;
  preg_match_all('/\[kaltura-widget(.*)\/\]/', $content, $matches);
  $kaltura_tags = array();

  foreach ($matches[0] as $key => $match) {

    if (strpos($match, '/][kal', $split_point) === FALSE) {
      $kaltura_tags[] = $matches[0][$key];
      continue;
    }
    $tags = explode('][', $match);
    $splitted = FALSE;
    if (count($tags)) {
      foreach ($tags as $tag_num => $tag) {
        $splitted = TRUE;
        if ($tag[0] != '[') {
          $tags[$tag_num] = '['. $tag;
        }
        if ($tag[drupal_strlen($tag) - 1] != ']') {
          $tags[$tag_num] .= ']';
        }
        $kaltura_tags[] = $tags[$tag_num];
      }
    }
  }

//get the node ID if available - needed to identify node access
/*if ( arg(0) == 'node' && is_numeric(arg(1)) ) {
  $node_id = arg(1);
}*/

  foreach ($kaltura_tags as $kaltura_tag) {

    $found = TRUE;
    // parse the parameters from the tag
    $params = kaltura_get_params_from_tag($kaltura_tag);
    // get the embed options from the params
    $embed_options = kaltura_get_embed_options($params);
    
    $wid = $embed_options["wid"];
    $width = $embed_options["width"];
    $height = $embed_options["height"];
    $div_id = "kaltura_wrapper_". $wid;
    $thumbnail_div_id = "kaltura_thumbnail_". $wid;
    $player_id = "kaltura_player_". $wid;
    $partner_config = KalturaHelpers::getServiceConfiguration();
    $kaltura_server = (variable_get('kaltura_server_url', ''))? variable_get('kaltura_server_url', ''): KalturaSettings_SERVER_URL;

    $kaltura_poweredby = '<div style="width: '. $width .'px;" class="poweredByKaltura">'.
    '<!--<div><a target="_blank" href="http://drupal.kaltura.org">Video Module</a> by <a target="_blank" href="http://corp.kaltura.com">Kaltura</a></div>-->'.
    '</div>';
    if (variable_get('kaltura_display_credits', 1)) {
      $links = '<a href="http://corp.kaltura.com/download">open source video</a><a href="http://corp.kaltura.com/technology/">video platform</a>';
    }
    if ($replace_to_thumb === TRUE) {
      $html = '<img src="'. $kaltura_server .'/p/'. $partner_config->partnerId .'/sp/'. $partner_config->subPartnerId .'/thumbnail/entry_id/'. $embed_options["media_id"] .'" />';
      $content = str_replace($kaltura_tag, $html, $content);
      continue;
    }
    
    $embed_textarea_id = "kaltura_embed_".$embed_options["media_id"];
    $div_id = "kaltura_wrapper_". $embed_options["media_id"];
    $player_id = ($embed_options['div_id'])? $embed_options['div_id']: "kaltura_player_". $embed_options["media_id"];
    $align = ($embed_options["align"])? 'text-align:'. $embed_options["align"] .'; ': '';
    if ($is_comment) {
      $thumb_div_id = 'kaltura_thumbnail_'. $embed_options["media_id"];
      $onclick_value = 'kaltura_activate_player(\''. $thumb_div_id .'\',\''. $div_id .'\');';
      $img_tag = '<img src="'. $kaltura_server .'/p/'. $partner_config->partnerId .'/sp/'. $partner_config->subPartnerId .'/thumbnail/entry_id/'. $embed_options["media_id"] .'/width/'. $width .'/height/'. $height .'/type/2/bgcolor/000000/crop_provider/wordpress_comment_placeholder" />';
      $comment_div = '<div id="'. $thumb_div_id .'" class="kaltura_hand" onclick="'. $onclick_value .'">';
      $comment_div .= $img_tag .'</div>';
    }
    else {
      $comment_div = '';
    }
  
    
    $html .= $comment_div .'
      <div id="'. $div_id .'" class="kaltura_wrapper" style="'. (($comment_div != '')? 'display:none;':'') . $align . $embed_options['custom_style'] .'"'. $embed_options['js_events'] .'>'. $links .'</div>'. $kaltura_poweredby;
     if ($show_embed) {
       if ($params['media_type'] == 'viewplaylist') $title_class="kaltura_embed_title";
        $html .= '<div class="kaltura_embed_code"><strong class="'.$title_class.'">Embed code of this video:</strong><br /><textarea style="overflow-y:auto" id="'.$embed_textarea_id.'" onclick="javascript:this.focus; this.select();"></textarea></div>'; 
     }
     
     
     //Render KDP
     $html .= '<script type="text/javascript">
              var kaltura_swf = new SWFObject("'. $embed_options["swfUrl"] .'", "'. $player_id .'", "'. $embed_options["width"] .'", "'. $embed_options["height"] .'", "9", "#000000");
              kaltura_swf.addParam("wmode", "opaque");
              kaltura_swf.addParam("flashVars", "'. $embed_options["flashVars"] . (($comment_div != '')? '&autoPlay=true': '') .'");
              kaltura_swf.addParam("allowScriptAccess", "always");
              kaltura_swf.addParam("allowFullScreen", "TRUE");
              kaltura_swf.addParam("allowNetworking", "all");
              kaltura_swf.write("'. $div_id .'");';
        if ($show_embed) {
     $html .= '           
                          user_embed_textbox = document.getElementById("'.$embed_textarea_id.'");
                  user_embed_textbox.value = kaltura_swf.getSWFHTML();
                        ';
        }
        if ($params['media_type'] == 'viewplaylist' &&  ($added_playlist == false))
        {
         $added_playlist = true;
     $html .= '           
                          function customFunc1(parm) {prev_playlist_item()}
                          function customFunc2(parm) {next_playlist_item();}
                        ';
        
        }
    $html .= '</script>
    ';
    
    
    
    // rebuild the html with our new code tag 
    $content = str_replace($kaltura_tag, $html, $content);
  }
  
  if ($found && $replace_to_thumb === FALSE) {
    $plugin_url = KalturaHelpers::getKalturaServerUrl();
    $js = '
      <script type="text/javascript">';
     

     $js .= '
        function onPlayerAddClick (kshowId,entryId,pd_extraData) {
          if (kshowId && kshowId != -1) 
            kalturaInitModalBox("'. url("kaltura/contribution_wizard/") .'" + kshowId);
          if (entryId && entryId != -1 && "true" == "'. (($embed_options['roughcut'])? 'true': '') .'")
            kalturaInitModalBox("'. url("kaltura/contribution_wizard/") . (($embed_options['roughcut'])? 'entry-': '') .'" + entryId );
        }';
  
        $js .= '
        function onPlayerEditClick (kshowId,entryId,pd_extraData) {
          if (kshowId && kshowId != -1 && "true" == "'. (($embed_options['kshow'])? 'true': '') .'") 
            kalturaInitModalBox("'. url($kaltura_editor_URL) .'/" + kshowId + "/kshow/user_id@'. $user->uid .'", { width: '.$kaltura_editor_width.', height: '.$kaltura_editor_height.' } );
          if (entryId && entryId != -1 && "true" == "'. (($embed_options['roughcut'])? 'true': '') .'")
            kalturaInitModalBox("'. url($kaltura_editor_URL) .'/" + entryId + "/entry/user_id@'. $user->uid .'", { width: '.$kaltura_editor_width.', height: '.$kaltura_editor_height.' } );
        }';
        
    //NOTE CM: new KDP v2 Editor handler
     
    if($edit_mix_access) {
                $js .= '
                        function gotoEditorWindow(entryID) {';
                
                //If the edit button is pressed on the Drupal node edit page, remind user that they still need to submit the node, especially if it's new, else they won't be able to access their content.
                if (arg(count(arg())-1) == 'edit') {
                        $js .= 'alert("WARNING: Edits are not saved until you submit this form.  You should submit first before editing your mix.");';
                }
                
                $js .= '
                                kalturaInitModalBox("'. url($kaltura_editor_URL) .'/" + entryID + "/entry/user_id@'. $user->uid .'", { width: '.$kaltura_editor_width.', height: '.$kaltura_editor_height.' });
                        }';
        } else {
                $js .= '
                        function gotoEditorWindow(entryID) {
                                if (entryID) {
                                        window.location = "'.url("kaltura/clone").'/"+entryID+"/'.$content_type.'/confirm";
                                }
                        }';
        }
  
        $js .= '
        function handleGotoContribWizard (kshowId, pd_extraData) {
          kalturaInitModalBox("'. url("kaltura/contribution_wizard/") .'" + kshowId);
        }';
        /*
        $js .= '
        function handleGotoEditorWindow (kshowId, pd_extraData) {
          kalturaInitModalBox("'. url($kaltura_editor_URL) .'/" + kshowId, { width: 890, height: 546 } );
        }';
   */  
   $js .= '</script>';
  
    $content .= $js;
    
  }

  return $content;
}

/*
 * helper function that breaks the "kaltura tag" into parameters
 */
function kaltura_get_params_from_tag($tag) {
  $params = array();
  $attributes_array = explode(' ', $tag);
  for ($i = 1, $len = count($attributes_array); $i < $len; $i++) {
    $attr = $attributes_array[$i];
    if (!strpos($attr, "=")) {
      continue;
    }
    $attr = str_replace('"', "", $attr);
    $attr = str_replace("'", "", $attr);
    $keyvalue = explode('=', $attr);
    $key = $keyvalue[0];
    $value = $keyvalue[1];
    $params[$key] = $value;
  }
  return $params;
}


/*
 * callback function for kaltura/contribution_wizard
 *
 * Args is url are a PHP array encoded as JSON and url encoded
 * use kaltura_format_cw_vars() function to convert your array of variables to the correct string
 *
 * this function exposes a hook call kaltura_use_cw which has higer priority then any other option
 *
 * valid list of params: (required params marked with *, values in brackets)
 *   kshow_id* [-2, -1, 0, real_id]
 *   partner_data [free text]
 *   type [entry/null]
 *   context [field/field_mix/comment/null]
 *   field_id
 *   title
 */
 function kaltura_contribution_wizard() {
  global $user;
  $args = func_get_args();
  $temp_args = unserialize(urldecode($args[0]));
  
  // argument handling
  
  foreach ($temp_args as $key => $val) {
   ${$key} = $val;
  }

  $temp_ui_conf = kaltura_invoke('use_cw', $temp_args);
  if (!empty($temp_ui_conf[0]))
  {
        $ui_conf = $temp_ui_conf[0];
  }
  $is_entry_node = false;
  if ($kshow_id == -2)
  {
        $is_entry_node = true;
  }
  
  $kaltura_client = KalturaHelpers::getKalturaClient();
      
  if (!$kaltura_client) {
    drupal_set_message('Failed to start Kaltura session. Please check your settings.', 'warning');
    echo theme('kaltura_maintenance_page', '<br /><a href="#" onclick="window.top.kalturaCloseModalBox()">Close</a>', TRUE);
   
   exit;
  }
  /* TODO: remove the creation of RC */
 
                if (isset($args[1]) && $args[1] != '') {
                  $title = $args[1];
                }
                
        // create roughcut at this point by API call,
        $session_user = KalturaHelpers::getSessionUser();
        
        if (empty($partner_data))
        {
                $partner_data = "user_id@". $user->uid;
                $partner_data .= "|kshow_exist@no";
        }
                        
/*      $partner_data = "user_id@". $user->uid;
        $partner_data .= "|kshow_exist@no";
*/      
        if ($context == 'field_mix' /*|| $context == 'kaltura_mix' || $context == 'kaltura_entry'*/) {
                $rc = new KalturaMixEntry;
                $rc->name =  $title;
                $rc->userId = $session_user->screenName;
                //$rc->type = ENTRY_TYPE_ROUGHCUT;
                //$rc->tags = $_REQUEST['kaltura_tags'];
                //$rc->adminTags = $_REQUEST['kaltura_admin_tags'];
                $rc->partnerId = variable_get('kaltura_partner_id', '');
                $rc->partner_data = $partner_data;
                
                //NOTE: CM TO DO - make SIMPLE/ADVANCED variable
                $rc->editorType = KalturaEditorType::ADVANCED;
                $result = $kaltura_client->mixing->add($rc);
                
                // pass roughcut ID to CW.
                $kshow_id = $result->id;
        } 

        //Don't create a node for field_mix
        if ($context != 'field_mix' && $is_entry_node == false && !empty($result)) {
                //NOTE: CM  - add an argument for updates vs. new node. right now this is causing two nodes to be created for every mix..
                kaltura_create_node_from_roughcut($result,1);
  }
  
  $theme_params->height = 360;
  $theme_params->width = 680;
  
  if ($context == 'comment')
  {
        $theme_params->swfUrl = KalturaHelpers::getContributionWizardUrl(KalturaSettings_CW_COMMENTS_UICONF_ID);
  }
  else
  {
        if (empty($ui_conf)) // ui_conf was not supplied by external vars (like block button)
        {
                $theme_params->swfUrl = KalturaHelpers::getContributionWizardUrl();
                if (empty($context) && is_entry_node == true) // this is entry
                {
                        $ui_conf = variable_get('kaltura_video_entry_cw_type', '');
                        if (!empty($ui_conf)) //found
                        {
                                if ($ui_conf == 1) // this is custom
                                {
                                        $ui_conf = variable_get('kaltura_video_entry_cw_custom', KalturaSettings_CW_UICONF_ID);
                                }
                                $theme_params->swfUrl = KalturaHelpers::getContributionWizardUrl($ui_conf);
                        }
                }
                else if ($context == 'kaltura_mix')
                {

                        $ui_conf = variable_get('kaltura_mix_roughcut_cw_type', '');
                        if (!empty($ui_conf)) //found
                        {
                                if ($ui_conf == 1) // this is custom
                                {
                                        $ui_conf = variable_get('kaltura_mix_roughcut_cw_custom', KalturaSettings_CW_UICONF_ID);
                                }
                                $theme_params->swfUrl = KalturaHelpers::getContributionWizardUrl($ui_conf);
                        }
                }
        }
        else
        {
                $theme_params->swfUrl = KalturaHelpers::getContributionWizardUrl($ui_conf);
        }
  }
  
  $simple = false;
  if (KalturaSettings_CW_UICONF_ID_SIMPLE == $ui_conf) $simple=true;
  
  $flash_vars = KalturaHelpers::getContributionWizardFlashVars($kaltura_client->getKs(), $kshow_id, $partner_data, $type, (($context == 'comment')? TRUE: FALSE), $simple, $ui_conf);
  $theme_params->flashVarsStr = KalturaHelpers::flashVarsToString($flash_vars);

  if ($partner_data /*&& strpos($partner_data, 'kshow_exist@no')*/) {
    $add_new = TRUE;
        $theme_params->mix_id = kaltura_get_node_for_mix($kshow_id);
  }


  if ($context == 'field' || $context == 'field_mix' || $context == 'comment') {
    $no_collect_entries = ($context == 'field_mix')? TRUE: FALSE;
    echo theme('kaltura_contribution_wizard_field', $theme_params, $field_id, $no_collect_entries, $kshow_id, $add_filter);
    exit();
  }
  if ($navigate_back === NULL) {
    $navigate_back = TRUE;
  }
  if (KalturaSettings_CW_UICONF_ID_SIMPLE == $ui_conf) {
    if (isset($write_output) && $write_output == 0)
    {
            return theme('kaltura_contribution_wizard_simple', $theme_params);
        }
        else
        {
            echo theme('kaltura_contribution_wizard_simple', $theme_params);
        }
  }
  else if ($kshow_id != "-2" && !$add_new) {
    echo theme('kaltura_contribution_wizard', $theme_params);
  }
  else {
    echo theme('kaltura_contribution_wizard_add', $theme_params, $navigate_back);
  }
  exit;
}

/*
 * theme the kaltura maintenance page
 *
 * this page is displayed whenever a try to create the $kaltura_client object fails
 */
function theme_kaltura_maintenance_page($text, $var) {
  return $text;
}

/*
 * theme for kaltura/contribution_wizard
 * in case we are adding entries to existing kshow
 */
function theme_kaltura_contribution_wizard($theme_params) {
  $goto_url = kaltura_cw_destination();
  $javascript = '
    var goto_url = "'. $goto_url .'";
    function onContributionWizardAfterAddEntry() {
    }
    
    function onContributionWizardClose(modified) {
      if (modified == "0") { 
        //setTimeout("window.top.history.back();", 0);
        setTimeout("window.top.kalturaCloseModalBox();", 0);
      } else { 
        setTimeout("window.top.kalturaRefreshTop();", 0);
        // setTimeout("SendTopToEntriesPage();",0);
      }
      return;
    }
  ';
    
  $flash_embed = '
    <div id="divKalturaCw"></div>
    <script type="text/javascript">
      var kso = new SWFObject("'. $theme_params->swfUrl .'", "kalturaCw", "'. $theme_params->width .'", "'. $theme_params->height .'", "9", "#000000");
      kso.addParam("flashVars", "'. $theme_params->flashVarsStr .'");
      kso.addParam("allowScriptAccess", "always");
      kso.addParam("allowFullScreen", "TRUE");
      kso.addParam("allowNetworking", "all");
      kso.write("divKalturaCw");
    </script>
  ';
  
  return theme("kaltura_modal", array("javascript" => $javascript, "flashEmbed" => $flash_embed));
}

/*
 * theme for kaltura/contribution_wizard_add
 * in case we add entries as a standalone nodes or a new roughcut
 */
function theme_kaltura_contribution_wizard_add($theme_params, $navigate_back = TRUE) {

  $goto_url = kaltura_cw_destination();
  $javascript = '
    var kshowId = -1;
    var goto_url = "'. $goto_url .'";
    var node_url = "'. (($theme_params->mix_id)? url("node/". $theme_params->mix_id): '') .'/edit";
    function onContributionWizardAfterAddEntry(obj) {
    var entries = "&entries=";
                if (obj && obj.length > 0)
                {
                        for (i=0; i < obj.length; i++)
                        {
                                entryId = (obj[i].uniqueID == null ? obj[i].entryId : obj[i].uniqueID);
                                entries += entryId + ",";
                        }
                }
                node_url = node_url + entries;
//      if (obj && obj.length > 0 && obj[0].kshowId)
//      kshowId = obj[0].kshowId;
    }
    
    function onContributionWizardClose(modified) {
      if (modified == "0")';
      $javascript .= ($theme_params->mix_id)? 'setTimeout("SendTopToNodePage();", 0);': ($navigate_back)? 'setTimeout("window.top.history.back();", 0);': 'setTimeout("window.top.kalturaCloseModalBox();",0);';
      $javascript .= ' else ';
      $javascript .= ($theme_params->mix_id)? 'setTimeout("SendTopToNodePage();", 0);': 'setTimeout("SendTopToEntriesPage();",0);';
      $javascript .= '}';
    
    
  $flash_embed = '
    <div id="divKalturaCw"></div>
    <script type="text/javascript">
      var kso = new SWFObject("'. $theme_params->swfUrl .'", "kalturaCw", "'. $theme_params->width .'", "'. $theme_params->height .'", "9", "#000000");
      kso.addParam("flashVars", "'. $theme_params->flashVarsStr .'");
      kso.addParam("allowScriptAccess", "always");
      kso.addParam("allowFullScreen", "TRUE");
      kso.addParam("allowNetworking", "all");
      kso.write("divKalturaCw");
    </script>
  ';

  return theme("kaltura_modal", array("javascript" => $javascript, "flashEmbed" => $flash_embed));
}

/*
 * theme for kaltura/contribution_wizard_simple
 * in case we add entries as a standalone nodes or a new roughcut
 */
function theme_kaltura_contribution_wizard_simple($theme_params, $navigate_back = TRUE) {

  $goto_url = kaltura_cw_destination();
  $javascript = '
    var kshowId = -1;
    var goto_url = "'. $goto_url .'";
    var node_url = "'. (($theme_params->mix_id)? url("node/". $theme_params->mix_id): '') .'/edit";     
        var flashObj;
        var delegate = {};
        var mediaTypeInput;
        
        //KSU handlers
        delegate.readyHandler = function()
        {       
                flashObj = document.getElementById("kalturaCw");';
                $javascript .= ($theme_params->mix_id)? '': 'flashObj.setMaxUploads(1);';
                $javascript .= '
                
        }

        delegate.selectHandler = function()
        {       
                flashObj.upload();
//              console.log("selectHandler()");         
//              console.log(flashObj.getTotalSize());
        }

        function setMediaType()
        {
                var mediaType = document.getElementById("mediaTypeInput").value; alert(mediaType);
//              console.log(mediaType);
                flashObj.setMediaType(mediaType);
        }
        
        delegate.singleUploadCompleteHandler = function(args)
        {
                
                flashObj.addEntries();
                console.log("singleUploadCompleteHandler", args[0].title);
                
        }

        delegate.allUploadsCompleteHandler = function()
        {
                console.log("allUploadsCompleteHandler");
        }

        delegate.entriesAddedHandler = function(entries)
        {
            var entriesStr = "&entries=";
                if (entries && entries.length > 0)
                {
                        for (i=0; i < entries.length; i++)
                        {
                                entryId = (entries[i].uniqueID == null ? entries[i].entryId : entries[i].uniqueID);
                                entriesStr += entryId + ",";
                        }
                }
                node_url = node_url + entriesStr;
                
                console.log(entries);
                document.getElementById("filStatus").innerHTML = "Upload Complete!";
                ';
                $javascript .= ($theme_params->mix_id)? 'setTimeout("SendTopToNodePage();", 0);': 'setTimeout("SendTopToEntriesPage();",0);';
        $javascript .= '}

        delegate.progressHandler = function(args)
        {       
                document.getElementById("simpleStatus").style.display="inline";
//              document.getElementById("divKalturaCw").style.display="none";
                document.getElementById("btnSimpleUpload").style.display="none";
                document.getElementById("fileName").innerHTML = args[2].title;
                document.getElementById("filSize").innerHTML = Math.round(args[1]/1000) + "KB";
                document.getElementById("filStatus").innerHTML = "Do not leave this page!!";
                document.getElementById("progress").innerHTML = Math.round((args[0]*100 / args[1])*100)/100 + "%";
                console.log(args[2].title + ": " + args[0] + " / " + args[1]);
        }
    
        delegate.uiConfErrorHandler = function()
        {
                console.log("ui conf loading error");
        }
    
    function onContributionWizardAfterAddEntry(obj) {
    var entries = "&entries=";
                if (obj && obj.length > 0)
                {
                        for (i=0; i < obj.length; i++)
                        {
                                entryId = (obj[i].uniqueID == null ? obj[i].entryId : obj[i].uniqueID);
                                entries += entryId + ",";
                        }
                }
                node_url = node_url + entries;
//      if (obj && obj.length > 0 && obj[0].kshowId)
//      kshowId = obj[0].kshowId;
    }
    
    function onContributionWizardClose(modified) {
      if (modified == "0")';
      $javascript .= ($theme_params->mix_id)? 'setTimeout("SendTopToNodePage();", 0);': ($navigate_back)? 'setTimeout("window.top.history.back();", 0);': 'setTimeout("window.top.kalturaCloseModalBox();",0);';
      $javascript .= ' else ';
      $javascript .= ($theme_params->mix_id)? 'setTimeout("SendTopToNodePage();", 0);': 'setTimeout("SendTopToEntriesPage();",0);';
      $javascript .= '}';
    
    
  $flash_embed = '

    <div id="divKalturaCw" style="position:absolute; top:130; left:240; z-index:999;"></div>
    <script type="text/javascript">
      var kso = new SWFObject("'. $theme_params->swfUrl .'", "kalturaCw", "'. '200' .'", "'. '30' .'", "9", "#000000");
      kso.addParam("flashVars", "'. $theme_params->flashVarsStr .'");
      kso.addParam("allowScriptAccess", "always");
      kso.addParam("allowFullScreen", "TRUE");
      kso.addParam("allowNetworking", "all");
      kso.addParam("wmode", "transparent");
      kso.write("divKalturaCw");
    </script>
                <div id="userInput" style="height:100px">
                <form>
                        <input type="button" id="btnSimpleUpload" style="position:absolute; top:130; left:240; margin:0;padding:0;" value="Click to upload">
                        <span style="position:absolute; top:230; left:240; margin:0;padding:0;width:200px"><div id="simpleStatus" style="position:absolute; margin-top:20px;margin-bottom:20px;display:none"><span id="fileName">&nbsp;</span><br/><span>Size: </span><span id="filSize">&nbsp;</span><br/><span>Progress: </span><span id="progress">&nbsp;</span><br/><span id="filStatus">&nbsp;</span></div></span>
                </form>
                </div>
    
  ';

  return theme("kaltura_modal", array("javascript" => $javascript, "flashEmbed" => $flash_embed));
}

/*
 * theme for kaltura/contribution_wizard_field
 * in case we add content as a CCK field
 */
function theme_kaltura_contribution_wizard_field($theme_params, $field_id, $no_collect_entries, $kshow_id, $add_filter = 1) {
  $goto_url = kaltura_cw_destination();
    
  $javascript = '
    var entry_list = '. (($no_collect_entries === TRUE)? '"'. $kshow_id .'"': '-1') .';
    var goto_url = "'. $goto_url .'";
    var node_url = "'. (($theme_params->mix_id)? url("node/". $theme_params->mix_id): '') .'";
    function onContributionWizardAfterAddEntry(obj) {
      str = "";
      for(i=0;i<obj.length;i++) {
        str += obj[i]["entryId"] + ",";
      }
      '.
      (($no_collect_entries === TRUE)? '': 'if (entry_list == "-1") entry_list = str; else entry_list += str;') .'
    }
    
    function onContributionWizardClose(modified) {
      if (modified == "0")
        setTimeout("window.top.kalturaCloseModalBox();", 0);
      else
        window.location.href = "'. url("kaltura/insert_widget/") .'" + entry_list + "/'. $field_id .'";
    }
  ';

  $add_existing = '';
  $js  = '';
  if ($field_id != 'edit-comment') {
    $js = '$(document).ready(function() { window.top.document.getElementById("kaltura_modal_iframe").className = ""; ';
    $js .= PHP_EOL .'window.top.document.getElementById("kaltura_modal_iframe").scrolling = "no"; });';
    $js .= PHP_EOL .'window.top.document.getElementById("modalbox").style.height = "380px";';
    $js .= PHP_EOL .'window.top.document.getElementById("kaltura_modal_iframe").height = "380";';
    $add_existing = '<div id="media-upload-header">
          <ul id="sidemenu">
          <li id="tab-kaltura"><a class="current" href="'. url($_GET['q']) .'">Add Interactive Video</a></li>
          <li id="tab-kaltura_browse"><a href="'. url('kaltura/entries/allow-insert/'. $field_id, array( 'query' => array( 'filter' => $add_filter, 'cwurl' => url($_GET['q']) ) )) .'">Browse Interactive Videos</a></li>
          <li class="close"><span class="close" href="#" onclick="window.top.kalturaCloseModalBox();return false;">&nbsp;&nbsp;&nbsp;&nbsp;</span></li>
          </ul>
      </div>';
  }
  $flash_embed = '<div id="divKalturaCw"></div>
    <script type="text/javascript">
      var kso = new SWFObject("'. $theme_params->swfUrl .'", "kalturaCw", "'. $theme_params->width .'", "'. $theme_params->height .'", "9", "#000000");
      kso.addParam("flashVars", "'. $theme_params->flashVarsStr .'");
      kso.addParam("allowScriptAccess", "always");
      kso.addParam("allowFullScreen", "TRUE");
      kso.addParam("allowNetworking", "all");
      kso.write("divKalturaCw");
    </script>
  ';

  return theme("kaltura_modal", array("javascript" => $javascript . $js, "flashEmbed" => $add_existing . $flash_embed));
}

/*
 * callback function for kaltura/simple_editor 
 */
function kaltura_simple_editor() {
  $args = func_get_args();
  $kshow_id = $args[0];
  $type = $args[1];
  $partner_data = ($args[2] != 'empty_partner_data')? $args[2]: '';
  $no_refresh = ($args[3] == 'no_refresh')? TRUE: FALSE;


  $kaltura_editor_width = (variable_get('kaltura_editor_width', ''))? variable_get('kaltura_editor_width', ''): KalturaSettings_SE_WIDTH;
  $kaltura_editor_height = (variable_get('kaltura_editor_height', ''))? variable_get('kaltura_editor_height', ''): KalturaSettings_SE_HEIGHT;
  $editor_uiConf = variable_get('kaltura_mix_roughcut_editor_type', '');
  if (!empty($editor_uiConf) && $editor_uiConf == KalturaSettings_SE_CUSTOM)
  {
        $editor_uiConf = variable_get('kaltura_mix_roughcut_editor_custom', '');
  }
  
  if (WORK_WITH_KSHOW) {
    $kaltura_client = KalturaHelpers::getKalturaClient(FALSE, "edit:". $kshow_id);
  }
  else {
    $kaltura_client = KalturaHelpers::getKalturaClient(FALSE, "edit:*");
  }

  
  if (!$kaltura_client) {
    drupal_set_message('Failed to start Kaltura session. Please check your settings.', 'warning');
    echo theme_kaltura_maintenance_page("<br /><a href=\"#\" onclick=\"window.top.kalturaCloseModalBox()\">Close</a>", TRUE);
    exit;
  }
  
  $theme_params->height = $kaltura_editor_height;
  $theme_params->width = $kaltura_editor_width;
  
  if (!empty($editor_uiConf))
  {
          $theme_params->swfUrl = KalturaHelpers::getSimpleEditorUrl($editor_uiConf);
          $flash_vars = KalturaHelpers::getSimpleEditorFlashVars($kaltura_client->getKs(), $kshow_id, $type, $partner_data, $editor_uiConf);
 }
  else
  {
          $theme_params->swfUrl = KalturaHelpers::getSimpleEditorUrl();
          $flash_vars = KalturaHelpers::getSimpleEditorFlashVars($kaltura_client->getKs(), $kshow_id, $type, $partner_data);  
  }
  
  $theme_params->flashVarsStr = KalturaHelpers::flashVarsToString($flash_vars);
        
  echo theme('kaltura_simple_editor', $theme_params, $no_refresh);
  exit;
}

/**
 * Theme for kaltura/simple_editor 
 */
function theme_kaltura_simple_editor($theme_params, $no_refresh) {
  $javascript =' 
  function onSimpleEditorBackClick(modified) {
    if (modified == 0) { 
      setTimeout("window.top.kalturaCloseModalBox();", 0);
    } else {
    ';
  if ($no_refresh === TRUE) {
    $javascript .= 'setTimeout("window.top.kalturaCloseModalBox();", 0);';
  }
  else {
    $javascript .= 'setTimeout("window.top.kalturaRefreshTop();", 0);';
  }
  $javascript .= '
    }
    return;
  }
  
  function onSimpleEditorSaveClick(modified) {
  
  }
  ';

  $flash_embed = '
    <div id="divKalturaSe"></div>
    <script type="text/javascript">
      var kso = new SWFObject("'. $theme_params->swfUrl .'", "kalturaSe", "'. $theme_params->width .'", "'. $theme_params->height .'", "9", "#000000");
      kso.addParam("flashVars", "'. $theme_params->flashVarsStr .'");
      kso.addParam("allowScriptAccess", "always");
      kso.addParam("allowFullScreen", "TRUE");
      kso.addParam("allowNetworking", "all");
      kso.write("divKalturaSe");
    </script>
  ';

  return theme("kaltura_modal", array("javascript" => $javascript, "flashEmbed" => $flash_embed));
}



/*
 * callback function for kaltura/advanced_editor 
 */
function kaltura_advanced_editor() {
  $args = func_get_args();
  $kshow_id = $args[0];
  $type = $args[1];
  $partner_data = ($args[2] != 'empty_partner_data')? $args[2]: '';
  $no_refresh = ($args[3] == 'no_refresh')? TRUE: FALSE;

  $kaltura_editor_width = (variable_get('kaltura_editor_width', ''))? variable_get('kaltura_editor_width', ''): KalturaSettings_AE_WIDTH;
  $kaltura_editor_height = (variable_get('kaltura_editor_height', ''))? variable_get('kaltura_editor_height', ''): KalturaSettings_AE_HEIGHT;

  $editor_uiConf = variable_get('kaltura_mix_roughcut_editor_type', '');
  if (!empty($editor_uiConf) && $editor_uiConf == KalturaSettings_AE_CUSTOM)
  {
        $editor_uiConf = variable_get('kaltura_mix_roughcut_editor_custom', '');
  }
  
  if (WORK_WITH_KSHOW) {
    $kaltura_client = KalturaHelpers::getKalturaClient(FALSE, "edit:". $kshow_id);
  }
  else {
    $kaltura_client = KalturaHelpers::getKalturaClient(FALSE, "edit:*");
  }

  
  if (!$kaltura_client) {
    drupal_set_message('Failed to start Kaltura session. Please check your settings.', 'warning');
    echo theme_kaltura_maintenance_page("<br /><a href=\"#\" onclick=\"window.top.kalturaCloseModalBox()\">Close</a>", TRUE);
    exit;
  }
  
  $theme_params->height = $kaltura_editor_height;
  $theme_params->width = $kaltura_editor_width;

  
  if (!empty($editor_uiConf))
  {
          $theme_params->swfUrl = KalturaHelpers::getAdvancedEditorUrl($editor_uiConf);
          $flash_vars = KalturaHelpers::getAdvancedEditorFlashVars($kaltura_client->getKs(), $kshow_id, $type, $partner_data, $editor_uiConf);
  }
  else
  {
          $theme_params->swfUrl = KalturaHelpers::getAdvancedEditorUrl();
          $flash_vars = KalturaHelpers::getAdvancedEditorFlashVars($kaltura_client->getKs(), $kshow_id, $type, $partner_data);
  }
   
  $theme_params->flashVarsStr = KalturaHelpers::flashVarsToString($flash_vars);
    
  echo theme('kaltura_advanced_editor', $theme_params, $no_refresh);
  exit;
}

/*
 * theme for kaltura/advanced_editor 
 */
function theme_advanced_editor($theme_params, $no_refresh) {
  $javascript =' 
  function onSimpleEditorBackClick(modified) {
    if (modified == 0) { 
      setTimeout("window.top.kalturaCloseModalBox();", 0);
    } else {
    ';
  if ($no_refresh === TRUE) {
    $javascript .= 'setTimeout("window.top.kalturaCloseModalBox();", 0);';
  }
  else {
    $javascript .= 'setTimeout("window.top.kalturaRefreshTop();", 0);';
  }
  $javascript .= '
    }
    return;
  }
  
  function onSimpleEditorSaveClick(modified) {
  
  }
  ';

        
  $flash_embed = '
    <div id="divKalturaAe"></div>
    <script type="text/javascript">
      var kso = new SWFObject("'. $theme_params->swfUrl .'", "kalturaAe", "'. $theme_params->width .'", "'. $theme_params->height .'", "9", "#000000");
      kso.addParam("flashVars", "'. $theme_params->flashVarsStr .'");
      kso.addParam("allowScriptAccess", "always");
      kso.addParam("allowFullScreen", "TRUE");
      kso.addParam("allowNetworking", "all");
      kso.write("divKalturaAe");
    </script>
  ';

  return theme("kaltura_modal", array("javascript" => $javascript, "flashEmbed" => $flash_embed));
}




/*
 * main theme for modal windows 
 */
function theme_kaltura_modal($theme_params) {
  return '
    <html>
    <head>
    <link rel="stylesheet" type="text/css" href="'. base_path() . drupal_get_path('module', 'kaltura') .'/style/kaltura.css"/>
    <style type="text/css">
            html, body { margin:0; padding:0; }
    </style>
    <script type="text/javascript" src="'. base_path() . drupal_get_path('module', 'kaltura') .'/js/kaltura.js"></script>
    <script type="text/javascript" src="'. base_path() .'misc/jquery.js"></script>
    <script type="text/javascript" src="'. base_path() . drupal_get_path('module', 'kaltura') .'/js/swfobject.js"></script>
    <script type="text/javascript">
      '. $theme_params["javascript"] .'    
    </script>
    </head>
    <body>
      '. $theme_params["flashEmbed"] .'
    </body>
    </html>';
}

/*
 * callback function for kaltura/insert_widget page
 * 
 * currently not in use
 * 
 * used in old version of the module.
 * might be useful for the "kaltura_inline" implementation
 */
function kaltura_insert_widget() {
  global $_kaltura_thumbnail_base_url;
  $args = func_get_args();
  $entry_list = $args[0];
  $field_name = str_replace('_', '-', $args[1]);
  $fix_subject = '';
  if ($field_name == 'edit-comment') {
    $fix_subject = '      if(window.top.document.getElementById("edit-subject")) {';
    $fix_subject .= '      if (window.top.document.getElementById("edit-subject").value == "") ';
    $fix_subject .= '       window.top.document.getElementById("edit-subject").value = "video comment";';
    $fix_subject .= ' }';
    $entries = explode(',', rtrim($entry_list, ','));
    $entry_list = '';
    foreach ($entries as $entry) {
      $entry_list .= '[kaltura-widget media_type=\"video\" comment=\"'. $entry .'\" size=\"large\" /]';
    }
  }
  
  $javascript =' 
    var bodyTextArea = window.top.document.getElementById("'. $field_name .'");
    if (bodyTextArea)
      bodyTextArea.value += "'. $entry_list .'";
      '. $fix_subject;
      if (module_exists('ajax_comments')) {
        $javascript .= ($field_name == 'edit-comment')? 'window.top.document.getElementById("ajax-comments-submit").click();': '';
      }
      else {
        $javascript .= ($field_name == 'edit-comment')? 'window.top.document.getElementById("edit-preview").click();': '';
      }      
      $javascript .= ($field_name != 'edit-comment')? 'update_field_thumbs(window.top.document.getElementById(\''. $field_name .'\'), \''. $_kaltura_thumbnail_base_url .'\');': '';
      $javascript .= '
      window.top.kalturaCloseModalBox();
  ';

  $flash_embed = '';
  
  echo theme("kaltura_modal", array("javascript" => $javascript, "flashEmbed" => $flash_embed));
  exit;
}

/*
 * helper function to get embed options according to the "kaltura tag" parameters
 *
 * this function also calls the kaltura_use_widget (file kaltura.module) function which exposes a hook
 *
 * returns an array of embed options later injected into the embed tag
 */
function kaltura_get_embed_options($params) {
  switch ($params["align"]) {
    case 'r': $align = "right";
      break;
    case 'm': $align = "center";
      break;
    case 'l': $align = "left";
      break;
    default: $align = "";
      break;
  }

  if ($params['id']) {
    $div_id = $params['id'];
  }
  if ($params['custom_style']) {
    $custom_style = $params['custom_style'];
  }
  
  if ($_SERVER["SERVER_PORT"] == 443) {
    $protocol = "https://";
  }
  else {
    $protocol = "http://"; 
  }

  if ($params['autoplay']) {
    $autoplay = '&autoplay=1';
  }
  if ($params['uiconf_id']) {
    $uicid = $params['uiconf_id'];
    if (is_numeric($uicid))
    {
            $theme_uicid = $uicid;
    }
    else
    {
            $theme_uicid = TRUE;
        }
  }
  
  if ($params['widget_id']) {
    $widget_id = $params['widget_id'];
  }
  $wid = '_'. variable_get('kaltura_partner_id', '');
  $delivery = 'HTTP';

  $widescreen = '';
  if ($params["entry"]) {
    $uicid = kaltura_use_uiconf($uicid, 'entry', $theme_uicid, $params['media_type']);
    $swf_url = KalturaHelpers::getSwfUrlForWidget($wid  .'/entry_id/'. $params["entry"] .'/uiconf_id/'. $uicid);
    $media_id = $params["entry"];
    if ($params['media_type'] == 'video') {
          $delivery = variable_get('kaltura_video_entry_delivery_type', 'HTTP');
      if (variable_get('kaltura_video_entry_player_ratio', 0) == 1) {
        $widescreen = '&widescreen=1';
      }
    }
    elseif ($params['media_type'] == 'viewplaylist')
    {
                $delivery = variable_get('kaltura_viewplaylist_entry_delivery_type', 'HTTP');
    }

    $player_size = kaltura_calculate_player_size('entry', $params['size'], $params['width'], $params['height']);
  }
  elseif ($params["kid"]) {
    $uicid = kaltura_use_uiconf($uicid, 'mix', $theme_uicid);
    $kshow = TRUE;
    $swf_url = KalturaHelpers::getSwfUrlForWidget($wid  .'/kid/'. $params["kid"] .'/uiconf_id/'. $uicid);
    $media_id = $params["kid"];
    $player_size = kaltura_calculate_player_size('mix', $params['size'], $params['width'], $params['height']);
  }
  elseif ($params["mix"]) {
    $uicid = kaltura_use_uiconf($uicid, 'mix', $theme_uicid);
    $roughcut = TRUE;
    $swf_url = KalturaHelpers::getSwfUrlForWidget($wid  .'/entry_id/'. $params["mix"] .'/uiconf_id/'. $uicid);
    $media_id = $params["mix"];
    $player_size = kaltura_calculate_player_size('mix', $params['size'], $params['width'], $params['height']);
    $delivery = variable_get('kaltura_roughcut_mix_delivery_type', 'HTTP');
  }
  elseif ($params["comment"]) {
    $uicid = kaltura_use_uiconf($uicid, 'entry', $theme_uicid, 'comment');
    $swf_url = KalturaHelpers::getSwfUrlForWidget($wid  .'/entry_id/'. $params["comment"] .'/uiconf_id/'. $uicid);
    $media_id = $params["comment"];
    $player_size = kaltura_calculate_player_size('comment', $params['size'], $params['width'], $params['height']);
      $delivery = variable_get('kaltura_comment_delivery_type', 'HTTP');
}

  $finish_f = '';
  if ($params['finishF']) {
    $finish_f = '&finishF='. $params['finishF'];
  }
  
  $flash_vars_str = KalturaHelpers::flashVarsToString(KalturaHelpers::getKalturaPlayerFlashVars(""));
  $flash_vars_str .= "&externalInterfaceDisabled=false&layoutId=". $player_size['layout_id'] ."&                pd_original_url=". urlencode($protocol . $_SERVER["HTTP_HOST"] . request_uri()) . $widescreen . $finish_f . $autoplay;
  if ($delivery == 'RTMP')
  {
    $flash_vars_str .= "&streamerType=rtmp&streamerUrl=rtmp://rtmpakmi.kaltura.com/ondemand&rtmpFlavors=1";
  }
  
  return array(
    "flashVars" => $flash_vars_str,
    "height" => $player_size["height"],
    "width" => $player_size["width"],
    "custom_style" => $custom_style,
    "align" => $align,
    "media_id" => $media_id,
    "js_events" => $events,
    "wid" => $wid,
    "uiconf" => $uicid,
    "roughcut" => $roughcut,
    "kshow" => $kshow,
    "swfUrl" => $swf_url,
    "div_id" => $div_id
  );
}

/*
 * helper function that returns the player width and height according to letter
 * found in the "kaltura tag"
 */
function kaltura_calculate_player_size($type, $size, $width = 0, $height = 0) {
  if ($width > 0 && $height > 0) {
    return array('width' => $width, 'height' => $height, 'layout_id' => 'fullLarge');
  }
  switch ($type) {
    case 'entry':
      $variable_width = variable_get('kaltura_entry_width', '410');
      $variable_height = variable_get('kaltura_entry_height', '364');
      break;
    case 'mix':
      $variable_width = variable_get('kaltura_mix_width', '410');
      $variable_height = variable_get('kaltura_mix_height', '364');
      break;
    case 'comment':
      $variable_width = variable_get('kaltura_comment_width', '250');
      $variable_height = variable_get('kaltura_comment_height', '244');
      break;
  }
  return array('width' => $variable_width, 'height' => $variable_height, 'layout_id' => 'fullLarge');
}
