<?php

/**
 * functions edited getSessionUser
 *
 *
 */
class KalturaHelpers
{
	function getContributionWizardFlashVars($ks, $kshowId, $partner_data, $type, $comment, $simple = false, $uiConf = KalturaSettings_CW_UICONF_ID)
	{
		$sessionUser = KalturaHelpers::getSessionUser();
		$config = KalturaHelpers::getServiceConfiguration();

		$flashVars = array();

		$flashVars["userId"] = $sessionUser->id;
    if (!$simple)
    {
  		$flashVars["sessionId"] = $ks;
    }
    else
    {
  		$flashVars["ks"] = $ks;
      $flashVars["entryId"] 	 = -1;	     
      $flashVars["jsDelegate"]   = "delegate"; 
      $flashVars["maxUploads"]   = 1; 
      $flashVars["subPId"]   = $config->subPartnerId; 
    }

		if ($sessionUserId == KalturaSettings_ANONYMOUS_USER_ID) {
			$flashVars["isAnonymous"] = true;
		}
			
		$flashVars["partnerId"] 	= $config->partnerId;
		$flashVars["subPartnerId"] 	= $config->subPartnerId;
		if ($kshowId)
		// TODO: change the following line for roughcut
		$flashVars["kshow_id"] 	= ($type == 'entry')? $type.'-'.$kshowId: $kshowId;
		else
		$flashVars["kshow_id"] 	= -2;

		$flashVars["afterAddentry"] 	= "onContributionWizardAfterAddEntry";
		$flashVars["close"] 		= "onContributionWizardClose";
		$flashVars["partnerData"] 	= $partner_data;

    if ($simple)
		 $flashVars["uiConfId"] 		= KalturaSettings_CW_UICONF_ID_SIMPLE;
		else if (!$comment)
		$flashVars["uiConfId"] 		= (empty($uiConf) ? KalturaSettings_CW_UICONF_ID : $uiConf);
		else
		$flashVars["uiConfId"] 		= KalturaSettings_CW_COMMENTS_UICONF_ID;
			
		$flashVars["terms_of_use"] 	= "http://corp.kaltura.com/tandc" ;
    
		return $flashVars;
	}


	function getSimpleEditorFlashVars($ks, $kshowId, $type, $partner_data, $uiConfId = null)
	{
		$sessionUser = KalturaHelpers::getSessionUser();
		$config = KalturaHelpers::getServiceConfiguration();


		$flashVars = array();

		if($type == 'entry')
		{
			$flashVars["entry_id"] 		= $kshowId;
			$flashVars["kshow_id"] 		= 'entry-'.$kshowId;
		} else {
			$flashVars["entry_id"] 		= -1;
			$flashVars["kshow_id"] 		= $kshowId;
		}

		$flashVars["partner_id"] 	= $config->partnerId;
		$flashVars["partnerData"] 	= $partner_data;
		$flashVars["subp_id"] 		= $config->subPartnerId;
		$flashVars["uid"] 			= $sessionUser->id;
		$flashVars["ks"] 			= $ks;
		$flashVars["backF"] 		= "onSimpleEditorBackClick";
		$flashVars["saveF"] 		= "onSimpleEditorSaveClick";
    if ($uiConfId)
  		$flashVars["uiConfId"] 		= $uiConfId;
    else
  		$flashVars["uiConfId"] 		= KalturaSettings_SE_UICONF_ID;
    
		return $flashVars;
	}
	
	
	function getAdvancedEditorFlashVars($ks, $kshowId, $type, $partner_data, $uiConfId = null)
	{
		$sessionUser = KalturaHelpers::getSessionUser();
		$config = KalturaHelpers::getServiceConfiguration();


		$flashVars = array();

		if($type == 'entry')
		{
			$flashVars["entry_id"] 		= $kshowId;
			$flashVars["kshow_id"] 		= 'entry-'.$kshowId;
		} else {
			$flashVars["entry_id"] 		= -1;
			$flashVars["kshow_id"] 		= $kshowId;
		}

		$flashVars["partner_id"] 	= $config->partnerId;
		$flashVars["partnerData"] 	= $partner_data;
		$flashVars["subp_id"] 		= $config->subPartnerId;
		$flashVars["uid"] 			= $sessionUser->id;
		$flashVars["ks"] 			= $ks;
		$flashVars["backF"] 		= "onSimpleEditorBackClick";
		$flashVars["saveF"] 		= "onSimpleEditorSaveClick";
    if ($uiConfId)
  		$flashVars["uiConfId"] 		= $uiConfId;
    else
  		$flashVars["uiConfId"] 		= KalturaSettings_AE_UICONF_ID;
    
		return $flashVars;
	}

	function getKalturaPlayerFlashVars($ks, $kshowId = -1, $entryId = -1)
	{
		$sessionUser = KalturaHelpers::getSessionUser();
//		$config = KalturaHelpers::getServiceConfiguration();

		$flashVars = array();

//		$flashVars["kshowId"] 		= $kshowId;
//		$flashVars["entryId"] 		= $entryId;
//		$flashVars["partner_id"] 	= $config->partnerId;
//		$flashVars["subp_id"] 		= $config->subPartnerId;
		$flashVars["uid"] 			= $sessionUser->id;
//		$flashVars["ks"] 			= $ks;

		return $flashVars;
	}

	function flashVarsToString($flashVars)
	{
		$flashVarsStr = "";
		foreach($flashVars as $key => $value)
		{
			$flashVarsStr .= ($key . "=" . urlencode($value) . "&");
		}
		return substr($flashVarsStr, 0, strlen($flashVarsStr) - 1);
	}

	function getSwfUrlForBaseWidget()
	{
		return KalturaHelpers::getSwfUrlForWidget(KalturaSettings_BASE_WIDGET_ID);
	}

	function getSwfUrlForWidget($widgetId)
	{
		return KalturaHelpers::getKalturaServerUrl() . "/kwidget/wid/" . $widgetId;
	}

	function getContributionWizardUrl($uiConfId = null)
	{
		if ($uiConfId)
    {
      if (KalturaSettings_CW_UICONF_ID_SIMPLE == $uiConfId)
      {
    		return KalturaHelpers::getKalturaServerUrl() . "/kupload/ui_conf_id/" . $uiConfId;
      }
      else
      {
    		return KalturaHelpers::getKalturaServerUrl() . "/kcw/ui_conf_id/" . $uiConfId;
      }  
    }
		else
		return KalturaHelpers::getKalturaServerUrl() . "/kcw/ui_conf_id/" . KalturaSettings_CW_UICONF_ID;
	}

	function getSimpleEditorUrl($uiConfId = null)
	{
		if ($uiConfId)
		return KalturaHelpers::getKalturaServerUrl() . "/kse/ui_conf_id/" . $uiConfId;
		else
		return KalturaHelpers::getKalturaServerUrl() . "/kse/ui_conf_id/" . KalturaSettings_SE_UICONF_ID;
	}
	
	function getAdvancedEditorUrl($uiConfId = null)
	{
		if ($uiConfId)
		return KalturaHelpers::getKalturaServerUrl() . "/kae/ui_conf_id/" . $uiConfId;
		else
		return KalturaHelpers::getKalturaServerUrl() . "/kae/ui_conf_id/" . KalturaSettings_AE_UICONF_ID;
	}

	function getThumbnailUrl($widgetId = null, $entryId = null, $width = 240, $height= 180)
	{
		$config = KalturaHelpers::getServiceConfiguration();
		$url = KalturaHelpers::getKalturaServerUrl();
		$url .= "/p/" . $config->partnerId;
		$url .= "/sp/" . $config->subPartnerId;
		$url .= "/thumbnail";
		if ($widgetId)
		$url .= "/widget_id/" . $widgetId;
		else if ($entryId)
		$url .= "/entry_id/" . $entryId;
		$url .= "/width/" . $width;
		$url .= "/height/" . $height;
		$url .= "/type/2";
		$url .= "/bgcolor/000000";
		return $url;
	}


	/**
	 * Initialise variables for the config object
	 * @return unknown_type
	 */
	static function getServiceConfiguration() {
		$partnerId = variable_get('kaltura_partner_id', 0);
		
		if($partnerId == '') $partnerId = 0;
		$subPartnerId = variable_get('kaltura_subp_id', 0);
		if($subPartnerId == '') $subPartnerId = 0;

		$config = new KalturaConfiguration();
		$config->serviceUrl = KalturaHelpers::getKalturaServerUrl();
		$config->subPartnerId = $subPartnerId;
		$config->partnerId = $partnerId;

		//$config->setLogger(new KalturaLogger());
		return $config;
	}

	/**
	 * get the url of the server either from drupal or the settings file
	 * @return unknown_type
	 */
	function getKalturaServerUrl() {
		$url = variable_get('kaltura_server_url', KalturaSettings_SERVER_URL);
		if($url == '') $url = KalturaSettings_SERVER_URL;

		// remove the last slash from the url
		if (substr($url, strlen($url) - 1, 1) == '/')
		$url = substr($url, 0, strlen($url) - 1);
		return $url;
	}

	/**
	 * CMAC
	 * gets the username and id of the current drupal user
	 * change: replaced KalturaSessionUser with KalturaUser object
	 * TODO: add more variables to the kaltura user object
	 * @return KalturaUser object
	 */
	function getSessionUser() {
		global $user;

		$kalturaUser = new KalturaUser();

		if ($user->uid) {
			$kalturaUser->id= $user->uid;
			$kalturaUser->screenName = $user->name;
			$kalturaUser->email = $user->mail;
				
		}
		else
		{
			$kalturaUser->id = KalturaSettings_ANONYMOUS_USER_ID;
		}

		return $kalturaUser;
	}

	/**
	 * oferc
	 * @return: the list of players defined for the account
	 */
  function getSitePlayers(&$arr)
  {
      static $players;
      
      $arr['48501'] = array('name' => 'Light', 'width' => 0, 'height' => 0);
      $arr['48502'] = array('name' => 'Dark', 'width' => 0, 'height' => 0);
      
      if (empty($players))
      {
        $players = array();
        $client = KalturaHelpers::getKalturaClient(true);
        $listResponse = $client->uiConf->listAction();
        for ($i=0; $i < $listResponse->totalCount; $i++)
        {
          if ($listResponse->objects[$i]->objType == KalturaUiConfObjType::PLAYER)
          {
            //Don't show playlist as regular player
            if (stristr($listResponse->objects[$i]->tags, "playlist") != FALSE)
            {
              continue;
            }
            $arr[$listResponse->objects[$i]->id] = array('name' => $listResponse->objects[$i]->name,
                                                         'width' => $listResponse->objects[$i]->width,
                                                         'height' => $listResponse->objects[$i]->height);
            $players[$listResponse->objects[$i]->id] = array('name' => $listResponse->objects[$i]->name,
                                                         'width' => $listResponse->objects[$i]->width,
                                                         'height' => $listResponse->objects[$i]->height);
   //         print($listResponse->objects[$i]->tags); //this is a KalturaUiConf object
          }
        }
     }
     else
     {
      foreach ($players as $key => $sitePlayer)
        {
             $arr[$key] = $sitePlayer;       
        }
     }
  }

	/**
	 * oferc
   * this method is defined just for clearence, acctualy it is the same as regular players
	 * @return: the list of players defined for the account
	 */
   
function getSitePlaylistPlayers(&$arr)
{
    $arr['1292302'] = array('name' => 'Playlist', 'width' => 0, 'height' => 0);
    return KalturaHelpers::getSitePlayers($arr);  
}

	function getKalturaClient($isAdmin = false, $privileges = null)
	{
		// get the configuration to use the kaltura client
		$kalturaConfig = KalturaHelpers::getServiceConfiguration();

		if(!$privileges) $privileges = 'edit:*';
		// inititialize the kaltura client using the above configurations
		$kalturaClient = new KalturaClient($kalturaConfig);
		// get the current logged in user
		$sessionUser = KalturaHelpers::getSessionUser();


		// get the variables requireed to start a session
		$partnerId = variable_get('kaltura_partner_id', '');
		$secret = variable_get('kaltura_secret', '');
		$adminSecret = variable_get('kaltura_admin_secret', '');


		if ($isAdmin)
		{
			$result = $kalturaClient->session->start($adminSecret, $sessionUser->id, KalturaSessionType::ADMIN, $partnerId,86400, $privileges);
		}
		else
		{
			$result = $kalturaClient->session->start($secret, $sessionUser->id, KalturaSessionType::USER, $partnerId, 86400, $privileges);
		}
		$len = strlen($result);
		/** proper method for error checking please
		if ($len!=116)
		{
			watchdog("kaltura", $result );
			return null;
		}else{
				*/
			// set the session so we can use other service methods
			$kalturaClient->setKs($result);
		//}

		return $kalturaClient;
	}
}


class KalturaContentCategories
{
	var $categories = array(
		'Arts & Literature',
		'Automotive',
		'Business',
		'Comedy',
		'Education',
		'Entertainment',
		'Film & Animation',
		'Gaming',
		'Howto & Style',
		'Lifestyle',
		'Men',
		'Music',
		'News & Politics',
		'Nonprofits & Activism',
		'People & Blogs',
		'Pets & Animals',
		'Science & Technology',
		'Sports',
		'Travel & Events',
		'Women',
	);
}
?>