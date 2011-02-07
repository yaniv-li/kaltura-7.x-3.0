<?php
// $Id: kaltura.admin.inc,v 1.1.2.5.2.10 2011/01/27 12:14:00 xuriz Exp $

/*
 * @file
 * contains functions for administration use of the kaltura core module
 *
 */

/*
 * helper function that process the partner registration with kaltura
 *
 * if registration successful, the partner details are saved as system variables (drupal DB) and
 * the user is being redirected to the modules settings page where he should see the details inserted
 * and a configuration test which says that the configuraion works
 * TODO set up for new API
 * CMAC
 * changes:
 * removed references to categories
 * questions: this method created problems within getServiceConfiguration wrt the Logger
 */
function kaltura_register_partner() {
  global $base_url;
  $partner = new KalturaPartner();
  $partner->name =              $_REQUEST['partner_name'];
  $partner->website =           $_REQUEST['partner_url1'];
  $partner->notificationUrl =   $_REQUEST['partner_url2'] .'/?q=kaltura/notification_handler';
  $partner->adminName =         $_REQUEST['partner_adminName'];
  $partner->adminEmail =        $_REQUEST['partner_adminEmail'];
  $partner->description =       $_REQUEST['partner_description'];
  $partner->commercialUse =     "non-commercial_use";
  $partner->type =              102;
  $partner->appearInSearch =    '1';

  // update relevant field with $_REQUEST['kaltura_adult_content']
  $content_categories = new KalturaContentCategories;
  $categories = "";
  if (!empty($_REQUEST['kaltura_content_categories']))
  {
    foreach ($_REQUEST['kaltura_content_categories'] as $selected) {
     $categories .= $content_categories->categories[$selected] .',';
    }
  }
  else
  {
    $categories="N/A";
  }
  $partner->adultContent = ($_REQUEST['kaltura_adult_content'] == 1)? true: false;

  $categories = rtrim($categories, ',');
  if (empty($categories)) $categories="N/A";
  $partner->contentCategories = $categories;
  /** OLD API
   $session_user = KalturaHelpers::getSessionUser();
   $config = KalturaHelpers::getServiceConfiguration();
   $kaltura_client = new KalturaClient($config);
   */
  $session_user = KalturaHelpers::getSessionUser();
  $config = KalturaHelpers::getServiceConfiguration();
  $kaltura_client = new KalturaClient($config);
//  $kaltura_client->setConfig($config);

  watchdog('regis', print_r($partner, TRUE));
  $res = $kaltura_client->partner->register($partner, $_REQUEST['cms_password']);
  debug($res);

  $result = $res->id;
  if ($result) {
    variable_set('kaltura_partner_id',         $res->id);
    variable_set('kaltura_subp_id',            $res->id."00");
    variable_set('kaltura_secret',             $res->secret);
    variable_set('kaltura_admin_secret',       $res->adminSecret);
    variable_set('kaltura_local_registration', 1);
    variable_set('kaltura_server_url',    KalturaHelpers::getKalturaServerUrl());
    variable_set('kaltura_partner_cms_email', $partner->adminEmail);
    variable_set('kaltura_partner_cms_password', $_REQUEST['cms_password']);
    variable_set('kaltura_partner_url2', $_REQUEST['partner_url2']);
    variable_set('kaltura_notification_type', 0);
    variable_set('kaltura_content_categories', $categories);

    kaltura_update_kaltura_partner(2, $partner->notificationUrl);
    kaltura_create_widgets_for_partner($partner->id, $secret);
    drupal_set_message('Congratulations!  You have successfully installed the All in One Video Module and registered for the Kaltura Partner Program.');
    drupal_goto('admin/config/kaltura/general');
  }
  else {
//    $msg = 'The email address you entered already belongs to a registered Kaltura partner.<br />'.
//      'Please register with another email or <a href="#" onclick="switch_to_exist_partner();return false;">click here</a> to import your existing partner details';
    $msg = 'There was an error while trying to register new partner.<br />'. str_replace('_', ' ', $res['error'][0]['code']) .': '. $res['error'][0]['desc'];
    drupal_set_message($msg, 'error');
  }
}

/*
 * helper function that process the partner registration for kaltura CE
 *
 * if registration successful, the partner details are saved as system variables (drupal DB) and
 * the user is being redirected to the modules settings page where he should see the details inserted
 * and a configuration test which says that the configuraion works
 */

function kaltura_import_ce() {
  try {
    $url = $_REQUEST['ce_url'];
    $email = $_REQUEST['ce_admin_email'];
    $password = $_REQUEST['ce_cms_password'];

    $kConfig = new KalturaConfiguration(0);
    $kConfig->serviceUrl = $url;
    $kClient = new KalturaClient($kConfig);
    $res = $kClient -> partner -> getSecrets(1, $email, $password);

    $partner = 1;
    $secret = $kPartner -> secret;
    $adminSecret = $kPartner -> adminSecret;

    variable_set('kaltura_partner_id',         $res->id);
    variable_set('kaltura_subp_id',            $res->id."00");
    variable_set('kaltura_secret',             $res->secret);
    variable_set('kaltura_admin_secret',       $res->adminSecret);
    variable_set('kaltura_local_registration', 1);
    variable_set('kaltura_server_url',    $url);
    variable_set('kaltura_partner_cms_email', $res->adminEmail);
    variable_set('kaltura_partner_cms_password', $password);
    variable_set('kaltura_partner_url2', $_REQUEST['partner_url2']);
    variable_set('kaltura_notification_type', 0);
//    variable_set('kaltura_content_categories', implode(',', $_REQUEST['kaltura_content_categories']));

    kaltura_update_kaltura_partner(2, $partner->notificationUrl);
//    kaltura_create_widgets_for_partner($partner->id, $secret);
    drupal_set_message('Congratulations!  You have successfully installed the All in One Video Module and registered for the Kaltura Partner Program.');
    drupal_goto('admin/settings/kaltura/general');
  }
  catch(Exception $ex) {
    drupal_set_message(t('Kaltura client exception: @message', array('@message' => $ex->getMessage())), 'error');
  }
}

/*
 * helper function to retrieve partner's details from kaltura
 * TODO: testing
 * CMAC changes:
 * kaltura client config set up to reflect changes in the API
 * getsecrets now called instead of getPartner
 * return array built using Kaltura Partner object
 *
 */
function kaltura_get_partner_details($partner_id, $cms_email, $cms_password) {

  $session_user = KalturaHelpers::getSessionUser();
  $config = KalturaHelpers::getServiceConfiguration();
  $config->partnerId = $partner_id;
  $kaltura_client = new KalturaClient($config);
//  $kaltura_client->setConfig();
  /**
   * here we are trying to retrieve the partner details from the kaltura server
   *
   */

  //$result = $kaltura_client->getPartner($session_user, $cms_email, $cms_password, $partner_id);
  $result = $kaltura_client->partner->getsecrets($partner_id, $cms_email, $cms_password);


  // convert result to array

  if ($result->id) {
    return array(
    $result->id,
    $result->id."00",//subpartner id
    $result->notificationUrl,
    $result->secret,
    $result->adminSecret
    );
  }
  else {
    return array(FALSE, $result['error']);
  }
}

/*
 * helper function that uses Kaltura's getDefaultWidget service to create some default widgets for
 * the partner, in case he does not have these yet
 * TODO
 * CMAC
 * changes
 * changed client initialisation
 * changed the getDefaultWidget function call to $kaltura_client->widget->get
 * question: what do we do with the widget when we get them?
 */
function kaltura_create_widgets_for_partner($partner_id, $secret) {
  /*$session_user = KalturaHelpers::getSessionUser();
   $config = KalturaHelpers::getServiceConfiguration();
   $config->partnerId = $partner_id;
   $kaltura_client = new KalturaClient();
   $kaltura_client->setConfig($config);*/

  $session_user = KalturaHelpers::getSessionUser();
  $kaltura_client = KalturaHelpers::getKalturaClient(1);

  $widgets = new KalturaSettings;

  foreach ($widgets->kdp_widgets as $type => $uiconfs) {
    foreach ($uiconfs as $skin => $confs) {
    /* oferc: that doesn't work for me yet */
//      $result = $kaltura_client->widget->get("_1_".$confs['view_uiconf']);
//      $result = $kaltura_client->widget->get("_1_".$confs['remix_uiconf']);
    }
  }
}

/*
 * determine how the general settings form will look like.
 *
 * returns one of 2 states of forms:
 * 1) registration form (if variables do not exist in the DB)
 *    - either as SaaS or Kaltura CE
 * 2) notifications settings + configuration test + settings form
 *
 */
function kaltura_settings_form() {
  global $base_url;

  if (!module_exists('advanced_help')) {
    $form['kaltura_help'] = array(
      '#value' => t('For more detailed help enable the advanced help module.'),
    );
  }

  if (variable_get('kaltura_partner_id', '') != '') {
    $notification_settings = kaltura_notifications_settings_form();
    $form['kaltura_notification_settings'] = array(
      '#type' => 'fieldset',
      '#title' => t('Kaltura to Drupal Notification Settings'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    foreach ($notification_settings as $name => $form_element) {
      $form['kaltura_notification_settings'][$name] = $form_element;
    }
    $form['kaltura_server_status'] = array(
      '#type' => 'fieldset',
      '#title' => t('Server Status'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE
    );
    list($admin_session_status, $admin_session_error) = kaltura_check_server_status($admin = 1);
    list($normal_session_status, $normal_session_error) = kaltura_check_server_status($admin = 0);
    $status_class = ($admin_session_status && $normal_session_status)? 'ok': 'bad';
    $session_status = ($admin_session_status && $normal_session_status)? 'OK': 'ERROR';

    $error = ' kaltura_error: ';
    if ($admin_session_error) {
      $error .= '<br />admin session: '. $admin_session_error;
      $error_status = TRUE;
    }
    if ($normal_session_error) {
      $error .= '<br />normal session: '. $normal_session_error;
      $error_status = TRUE;
    }

    $form['kaltura_server_status']['kaltura_Admin_test_server'] = array(
      '#type' => 'item',
      '#title' => t('Drupal to Kaltura Session Test'),
      '#prefix' => '<div class="kaltura_status_'. $status_class .'">',
      '#value' => 'Session status: '. $session_status . (($error_status)? $error: ''),
      '#suffix' => '</div>'
      );
      if (kaltura_variable_get_real('kaltura_notification_type', 0) == 1) {
        list($status, $text) = kaltura_test_notification_config();
        $prefix = '<div class="kaltura_status_'. (($status)? 'ok': 'bad') .'">';
        $form['kaltura_server_status']['kaltura_notification_status'] = array(
        '#type' => 'item',
        '#title' => t('Kaltura to Drupal Notification Status'),
        '#prefix' => $prefix,
        '#value' => $text,
        '#suffix' => '</div>'
        );
      }
      list($status, $text) = kaltura_test_cron_job_status();
      $prefix = '<div class="kaltura_status_'. (($status)? 'ok': 'bad') .'">';
      $form['kaltura_server_status']['kaltura_cron_job_status'] = array(
      '#type' => 'item',
      '#title' => t('Cron-Job Status'),
      '#prefix' => $prefix,
      '#value' => $text,
      '#suffix' => '</div>'
      );
      list($status, $text) = kaltura_test_crossdomain();
      $prefix = '<div class="kaltura_status_'. (($status)? 'ok': 'bad') .'">';
      $form['kaltura_server_status']['kaltura_crossdomain_status'] = array(
      '#type' => 'item',
      '#title' => t('CrossDomain.xml Status'),
      '#prefix' => $prefix,
      '#value' => $text,
      '#suffix' => '</div>'
      );
  }

  $register_visible = FALSE;
  if ($_REQUEST['partner_name'] && $_REQUEST['op'] == 'Complete Installation') {
    if ($_REQUEST['kaltura_agree_terms'] == 1) {
      // no test or error handling?
      kaltura_register_partner();
    }
    else {
      drupal_set_message('You must agree to the terms of use', 'error');
    }
  }

  if ($_REQUEST['op'] == 'Complete Installation' && $_REQUEST['ce_url'])
  {
    // TODO: If this fails, we need to display the server registration
    // form again.
    kaltura_import_ce();
  }

  /**
   * if the user already has a partner id, the importing of partner details is done here
   */

  if ($_REQUEST['op'] == 'Complete Installation' && $_REQUEST['kaltura_partner_id']) {
    list($pid, $subpid, $url2, $secret, $admin_secret) = kaltura_get_partner_details($_REQUEST['kaltura_partner_id'], $_REQUEST['cms_email'], $_REQUEST['cms_password']);
    if ($pid) {
      variable_set('kaltura_partner_id', $pid);
      variable_set('kaltura_subp_id', $subpid);
      variable_set('kaltura_secret', $secret);
      variable_set('kaltura_admin_secret', $admin_secret);
      variable_set('kaltura_partner_url2', str_replace('/?q=kaltura/notification_handler', '', $url2));
      variable_set('kaltura_partner_cms_email', $_REQUEST['cms_email']);
      variable_set('kaltura_partner_cms_password', $_REQUEST['cms_password']);
      variable_set('kaltura_server_url',    KalturaHelpers::getKalturaServerUrl());
      drupal_set_message('Your partner details were imported successfully. please check the '. l('notification settings', 'admin/settings/kaltura/general/notifications'));
      drupal_set_message('You can '. l('import Kaltura Media from your existing partner', 'admin/settings/kaltura/import'));
      kaltura_create_widgets_for_partner($pid, $secret);
      kaltura_update_kaltura_partner(2, $url2);
      drupal_goto('admin/settings/kaltura/general');
    }
    else {
      drupal_set_message('Couldn\'t import partner details.<br />'. print_r($subpid, TRUE) . $subpid['code'] .': '. $subpid['desc'], 'error');
    }
  }
  if (variable_get('kaltura_partner_id', '') == '') {
    $register_visible = TRUE;

    if ( empty($_REQUEST['op']))
    {
      if ($_GET['register'] !== 'no') {
        drupal_set_title('All in One Video Module by Kaltura Installation');
        // registration form
        $showing_reg = TRUE;
      $saasText = '<b>Using Kaltura SaaS hosted solution</b><br/> Kaltura service is responsible for transcoding your content
            to FLV, hosting it in a first grade CDN and streaming it to your viewers.<br/><br/>Kaltura Service has cost effective pricing and <b>10 GB of free trial</b>
            <br/><a href="http://www.kaltura.com" target="_blank">To learn more about the Kaltura SaaS offering and pricing click here</a><br/><br/>';
      $ceText ='<b>Using Kaltura Community Edition Beta</b><br/>Kaltua Comunity Edition will allow you to host, transcode and stream everything out of your own server for free.
             <br/><br/><b>This requires download, installation and configuration of the Kaltura CE server.</b><br/><br/><br/>';
      $form['kaltura_registration_mode'] = array(
      '#type' => 'radios',
      '#title' => t('Kaltura module has two modes of work'),
      '#options' => array(1 => $saasText, 0 => $ceText),
      '#default_value' => 1,
        );
      $form['kaltura_registration']['buttons']['import_partner'] = array(
          '#type' => 'button',
        '#title' => 'Continue >>',
      '#value' => 'Continue >>',
      '#default_value' => 'Continue >>',
      '#weight' => 40,
      );
      }


    }
    else
    {
      if (!variable_get('kaltura_registration_first_load', FALSE)) {
        variable_set('kaltura_registration_first_load', TRUE);
      }
      global $user;
      if($_REQUEST['op'] == 'Continue >>' && $_REQUEST['kaltura_registration_mode'] == 0)
      {
        if ($_GET['register'] !== 'no') {
          drupal_set_title('All in One Video Module by Kaltura Installation');
          // registration form
          $showing_reg = TRUE;
        $information_text = 'For the Drupal module to work with your Kaltura CE installation you must fill in the following <br/>details regarding your kaltura CE server. ';
        $notice_text = 'You must have a Kaltura Community Edition server(Kaltura CE) installed in order to continue.<br/><br/>1. Download Kaltura CE from <a href="http://www.kaltura.org/project/community_edition_video_platform" target="blank">Kaltura.org</a><br/><br/>2. Install Kaltura CE on your server based on the <a href="http://www.kaltura.org/kalturace-kaltura-community-edition-getting-started-guide" target="_blank">installation guide</a>';
        $form['kaltura_settings_information'] = array(
        '#prefix' => '<div class="kaltura_ce_notice">'.$notice_text.'</div>',
        '#value' => t($information_text)
        );

          $form['ce_url'] = array(
        '#type' => 'textfield',
        '#title' => t('Kaltura CE URL'),
        '#default_value' => '',
        '#size' => 100,
        '#maxlength' => 512
          );

          $form['ce_admin_email'] = array(
        '#type' => 'textfield',
        '#title' => t('Kaltura CE Admin Email'),
        '#default_value' => $user->mail,
        '#size' => 100,
        '#maxlength' => 100
          );

          $form['ce_cms_password'] = array(
        '#type' => 'textfield',
        '#title' => t('Kaltura CE Password'),
        '#default_value' => '',
        '#size' => 100,
        '#maxlength' => 100
          );
          $form['partner_url2'] = array(
          '#type' => 'hidden',
          '#title' => t('URL 2'),
          '#default_value' => $base_url,
          '#maxlength' => 1024
          );

          $form_state['storage']['submit'] = 'CE Register';
          $form['buttons']['register'] = array(
        '#type' => 'button',
        '#title' => 'Complete Installation',
        '#value' => 'Complete Installation',
        '#default_value' => 'Complete Installation',
        '#weight' => 40,
          );

          $switch_to_saas = '<a style="margin-left:50px;" href="#" onclick="switch_to_saas();return false;">Switch to Kaltura SaaS</a>';
          $form['buttons']['switch_to_saas'] = array(
           '#value' => t($switch_to_saas),
        '#weight' => 60,
        );
        }
      }
      else
      {
        $information_text = 'Once you complete the form below and click "Complete installation", '.
        'the All in One Video Module will be fully installed and ready to use.<br />'. PHP_EOL ;
        $registered = '<br /><a href="#" onclick="switch_to_exist_partner();return false;">Click here if you already have a Partner ID</a>';
        $not_registered = 'If you are not registered as a Kaltura partner, and would like to register '.
        '<input type="button" onclick="switch_to_register();" value="Click Here" />';

        $form['kaltura_settings_information'] = array(
        '#value' => t($information_text) . $registered
        );

        if ($_GET['register'] !== 'no') {
          drupal_set_title('All in One Video Module by Kaltura Installation');
          // registration form
          $showing_reg = TRUE;
          $form['kaltura_registration'] = array(
        '#type' => 'fieldset',
        '#title' => t('Registration'),
        '#collapsible' => FALSE,
        '#collapsed' => FALSE
          );
          $form['kaltura_registration']['partner_name'] = array(
        '#type' => 'textfield',
        '#title' => t('Website Name'),
        '#default_value' => variable_get('site_name', ''),
        '#size' => 100,
        '#maxlength' => 256
          );
          $form['kaltura_registration']['partner_url1'] = array(
          '#type' => 'hidden',
          '#title' => t('Your Website Address'),
          '#default_value' => $base_url,
          '#size' => 100,
          '#maxlength' => 1024
          );
          $form['kaltura_registration']['partner_url2'] = array(
          '#type' => 'hidden',
          '#title' => t('URL 2'),
          '#default_value' => $base_url,
          '#maxlength' => 1024
          );
          $form['kaltura_registration']['partner_adminName'] = array(
          '#type' => 'textfield',
          '#title' => t('Administrator Name'),
          '#default_value' => $user->name,
          '#size' => 100,
          '#maxlength' => 50
          );
          $form['kaltura_registration']['partner_adminEmail'] = array(
          '#type' => 'textfield',
          '#title' => t('Administrator Email'),
          '#default_value' => $user->mail,
          '#size' => 100,
          '#maxlength' => 100
          );
         $categories = new KalturaContentCategories();


          ////////////////////////////////////

          $form['kaltura_registration']['kaltura_content_categories'] = array(
           '#type' => 'select',
           '#title' => t('Content Categories'),
           '#multiple' => TRUE,
           '#size' => 8,
           '#options' => $categories->categories,
          );
          $form['kaltura_registration']['kaltura_adult_content'] = array(
        '#type' => 'radios',
        '#title' => t('Do you plan to display adult content?'),
        '#options' => array(1 => 'Yes', 0 => 'No'),
        '#default_value' => (variable_get('kaltura_adult_content', 0) == 0)? 0: 1,
          );
          $form['kaltura_registration']['partner_description'] = array(
          //'#type' => 'textarea',
          '#type' => 'hidden',
          '#title' => t('Partner Description'),
          '#default_value' => 'Drupal module|'. VERSION,
          );
          $form['kaltura_registration']['cms_password'] = array(
          '#type' => 'hidden',
          '#title' => t('CMS password'),
          '#default_value' => kaltura_generate_cms_password(),
          '#size' => 20,
          '#maxlength' => 60
          );
          $form['kaltura_registration']['kaltura_agree_terms'] = array(
        '#type' => 'checkbox',
        '#title' => 'I agree to comply with the <a href="http://www.kaltura.com/index.php/corp/tandc" target="_blank">Kaltura Terms of Use</a>',
        '#options' => 'Agree',
        '#required' => TRUE,
          );
          $form_state['storage']['submit'] = 'Register';
          $form['kaltura_registration']['buttons']['register'] = array(
        '#type' => 'button',
        '#title' => 'Complete Installation',
        '#value' => 'Complete Installation',
        '#default_value' => 'Complete Installation',
        '#weight' => 40,
          );
        }
      }
    }
  }
  if ($_GET['register'] === 'no' || (!$showing_reg && !isset($_GET['register']))) {
    // normal settings form
    if (variable_get('kaltura_partner_id', '') == '') {
      drupal_set_title('All in One Video Module by Kaltura Installation');
      $forgot_link_attributes = array(
        'attributes' => array(
          'target' => '_blank'
          )
          );
          $information_text = 'Please enter your CMS email & password';
          $information_text .= '<br />'. l('forgot password', 'http://www.kaltura.com/index.php/cms/login', $forgot_link_attributes);
          $form['kaltura_settings_information']['#value'] = $information_text;
    }
    $form['kaltura_partner_settings'] = array(
      '#type' => 'fieldset',
      '#title' => t('Partner Info'),
      '#collapsible' => ($_GET['register'] === 'no')? FALSE: TRUE,
      '#collapsed' => ($_GET['register'] === 'no')? FALSE: TRUE,
    );
    /*
     $form['kaltura_partner_settings']['kaltura_exitss_switch'] = array(
     '#value' => $not_registered,
     );
     */
     $partner_exist = variable_get('kaltura_partner_id', '');
     $readonly_attr = array();
     if (!empty($partner_exist))
     {
      $readonly_attr = array('readonly' => 'readonly');
     }

    $form['kaltura_partner_settings']['kaltura_partner_id'] = array(
      '#type' => 'textfield',
      '#title' => t('Partner ID'),
      '#default_value' => variable_get('kaltura_partner_id', ''),
      '#size' => 20,
      '#maxlength' => 10,
      '#attributes' => $readonly_attr
    );
    $form['kaltura_partner_settings']['cms_email'] = array(
      '#type' => 'textfield',
      '#title' => t('Email'),
      '#default_value' => variable_get('kaltura_partner_cms_email', ''),
      '#attributes' => $readonly_attr,
    );
    $form['kaltura_partner_settings']['cms_password'] = array(
      '#type' => 'textfield',
      '#title' => t('Password'),
      '#default_value' => variable_get('kaltura_partner_cms_password', ''),
        '#attributes' => $readonly_attr,
    );
    if (!empty($partner_exist))
    {
      $form['kaltura_partner_settings']['kaltura_upd_data'] = array(
      '#type' => 'item',
      '#value' => t('You can update your email and password via Kaltura Management Console here: ') . l('http://www.kaltura.com/index.php/kmc', 'http://www.kaltura.com/index.php/kmc', array( 'attributes' => array( 'target' => '_blank' ) ))
      );
    }
    if (variable_get('kaltura_partner_id', '') == '') {
      $form_state['storage']['submit'] = 'ImportPartner';
      $form['kaltura_registration']['buttons']['import_partner'] = array(
        '#type' => 'button',
        '#title' => 'Complete Installation',
        '#value' => 'Complete Installation',
        '#default_value' => 'Complete Installation',
        '#weight' => 40,
      );
    }
    $form['kaltura_partner_settings']['kaltura_subp_id'] = array(
      '#type' => 'hidden',
      '#title' => t('Kaltura Sub-Partner Id'),
      '#default_value' => variable_get('kaltura_subp_id', ''),
      '#size' => 20,
      '#maxlength' => 10
    );
    if (variable_get('kaltura_local_registration', 0) != 1 || 1) {
      $form['kaltura_partner_settings']['kaltura_secret'] = array(
        '#type' => 'hidden',
        '#title' => t('Kaltura Secret'),
        '#default_value' => variable_get('kaltura_secret', ''),
        '#size' => 60,
        '#maxlength' => 64
      );

      $form['kaltura_partner_settings']['kaltura_admin_secret'] = array(
        '#type' => 'hidden',
        '#title' => t('Kaltura Admin Secret'),
        '#default_value' => variable_get('kaltura_admin_secret', ''),
        '#size' => 60,
        '#maxlength' => 64
      );
    }
    $form['kaltura_partner_settings']['kaltura_server_url'] = array(
      '#type' => 'hidden',
      '#title' => t('Kaltura Server URL'),
      '#default_value' => variable_get('kaltura_server_url', KalturaHelpers::getKalturaServerUrl()),
      '#size' => 100,
      '#maxlength' => 64
    );
  }

  $form['kaltura_credits'] = array(
     '#type' => 'fieldset',
     '#title' => 'Credits',
     '#collapsible' => TRUE,
     '#collapsed' => TRUE,
  );
  $text = '<b>Open Source Video Links</b>'. PHP_EOL;
  $text .= 'Kaltura invests a great deal of effort in the open source community, and specifically in the open source video movement.';
  $text .= 'The following two links will be included within the video player in order to further promote Kaltura and its efforts in the open video space. ';
  $text .= 'We hope that you will join us by including these links in the player.'. PHP_EOL;
  $text .= l('open source video', 'http://corp.kaltura.com/download') . PHP_EOL . l('video platform', 'http://corp.kaltura.com/technology/');

  $form['kaltura_credits']['kaltura_credits_text'] = array(
    '#type' => 'item',
    '#value' => nl2br($text),
  );
  $form['kaltura_credits']['kaltura_display_credits'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include links in player'),
    '#option' => t('Include links in player'),
    '#default_value' => variable_get('kaltura_display_credits', 1),
  );

  $form['kaltura_help_message'] = array(
    '#type' => 'item',
    '#value' => t('Please feel free to ') . l('contact us', 'http://corp.kaltura.com/support/form/project/16', array( 'attributes' => array( 'target' => '_blank' ) )) . t(' with any questions.')
  );
  return system_settings_form($form);
}

/*
 * Implementation of hook_form_alter().
 */
function kaltura_form_alter(&$form, &$form_state, $form_id) {
  if (variable_get('kaltura_partner_id', '') == '') {
    if ($form_id == 'kaltura_settings_form') {
      unset($form['buttons']['submit']);
      unset($form['buttons']['preview']);
      unset($form['buttons']['reset']);
    }
  }
}

/*
 * callback function for url admin/settings/kaltura/import
 *CMAC
 *changes
 * KalturaEntryFilter changed to KalturaMediaEntryFilter
 * this may need some reworking
 */
function kaltura_import_entries_page() {
  // initialise variables
  $foundFlag = true;      // set to false if there are no entries
  $enabledFlag = false;     // set to true if mix or media node is enabled
  $entries = array();     // mix entries
  $mediaEntries = array();  // media entries
  $entries_need_import = array();
  $each_page=30;
  $total = 0;
  $mediaTotal = 0;
  $finished = false;
  $current_page = 1;

  if ($_POST['op'] == 'import selected') {
    kaltura_import_entries($_POST['entries_to_import']);
    drupal_goto('admin/settings/kaltura/import');
  }
  if (variable_get('kaltura_partner_id', '') == '') {
    return t('You have not configured your partner details yet. Import cannot be performed until you setup your partner details');
  }
  $get_entries = module_exists('node_kaltura_entry');
  $get_mixes = module_exists('node_kaltura_mix');

  if (empty($_GET['p'])) {
    $_GET['p'] = 1;
  }
  else
  {
    $current_page = $_GET['p'];
  }

  while (!$finished)
  {
    set_time_limit(30);
    //get the mix entries
    if ($get_mixes) {
      $filter = new KalturaMixEntryFilter();
      list($total, $entries) = kaltura_get_my_entries($filter, KalturaEntryType::MIX, $each_page, $current_page);
      if($total==0)$foundFlag = false;
      $enabledFlag = true;
    }
    // get the media entries
    if ($get_entries) {
      $filter = new KalturaMediaEntryFilter();
      list($mediaTotal, $mediaEntries) = kaltura_get_my_entries($filter, KalturaEntryType::MEDIA_CLIP, $each_page, $current_page);
      if($mediaTotal==0)$foundFlag=false;
      $enabledFlag = true;
    }
    if(!$enabledFlag) {
      return t('Can\'t import entries, you did not enable the node_kaltura_entries module.');
    }

    $entries_need_import = array_merge($entries_need_import, $entries, $mediaEntries);

    //list($total, $each_page, $entries) = kaltura_get_my_entries($filter, $each_page = 30, $_GET['p']);
    if ($entries === NULL) {
      return t('No entries found.');
    }
    $entries_need_import = kaltura_remove_existing_entries($entries_need_import);

    if (count($entries_need_import) >= $each_page)
    {
      $finished = true;
    }

    if (($mediaTotal < $current_page * $each_page) && ($total < $current_page * $each_page))
    {
      $finished = true;
    }
    $current_page++;
  }

  if (!count($entries_need_import)) {
    return t('No entries needed to be imported.');
  }

  if (count($entries_need_import) > $each_page)
  {
    $multi_dim = array_chunk  ( $entries_need_import  , $each_page, true );
    $entries_need_import = $multi_dim[0];
  }
/*
  if (count($entries_need_import) < $each_page) {
    list($second_count, $second_page) = kaltura_get_my_entries($filter, $each_page, $_GET['p'] + 1);
    $new_entries_need_import = kaltura_remove_existing_entries($second_page);
    if (is_array($new_entries_need_import) && $second_page !== NULL) {
      foreach ($new_entries_need_import as $eid => $entry) {
        $entries_need_import[$eid] = $entry;
        if (count($entries_need_import) == $each_page) {
          break;
        }
      }
    }
  }
*/
  if (($total+$mediaTotal) > $each_page) {
    $pages = ceil(($total+$mediaTotal)/$each_page);
    for ($start = 1;$start <= $pages;$start++) {
      if ($start == $_GET['p']) {
        $pager .= '<a>'. $start .'</a>';
      }
      else {
        $pager .= l($start, 'admin/settings/kaltura/import', array('query' => 'p='. $start));
      }
      $pager .= '&nbsp;';
    }
  }
  if (is_array($entries_need_import) && count($entries_need_import)) {
    foreach ($entries_need_import as $entry => $details) {
      $entries_options[$details['id']] = $details['id'] .': '. $details['name'] .' ('. (($details['type'] == 2)? 'mix': 'entry') .')';
    }
  }
  else {
    $entries_options = array();
  }
  $output = drupal_get_form('kaltura_create_entries_form', array($entries_options));
  return $pager . $output . $pager;
}











/*
 * helper function that uses kaltura_get_entries() to retreive a list of specific entries
 * belongs to the partner, and call kaltura_import_entry() for each of the retreived list
 */
function kaltura_import_entries($entries) {
  $entries_list = implode(',', $entries);
  $my_entries = kaltura_get_entries($entries_list, count($entries), true);

  foreach ($my_entries->objects as $entry) {
    kaltura_import_entry($entry);
  }
  drupal_set_message('<b>Please Note: All imported nodes are not yet published (node status 0)</b>');
}

/*
 * helper function that gets an entry retreived from Kaltura
 * builds a node object according to entry, and invokes node_insert to save the node
 */
function kaltura_import_entry($entry_details) {
  global $user;
  $node = new stdClass();
  if ($entry_details->type == 1) {
    $type = 'entry';
  }
  elseif ($entry_details->type == 2) {
    $type = 'mix';
  }
  else {
    $type = '';
  }
  $node->type = 'kaltura_'. $type;
  $node->title = $entry_details->name;
  $node->kaltura_entryId = $entry_details->id;
  $node->uid = $user->uid;
  $node->body = '';
  $node->status = 0;
  $node->created = time();
  $node->comment = FALSE;
  $node->changed = time();
  $node->promote = 0;
  $node->sticky = 0;
  $node->kstatus = $entry_details->status;
  $node->kaltura_entry_data = serialize($entry_details);
  $node->kaltura_tags = $entry_details->tags;
  $node->kaltura_admin_tags = '';
  $node->kaltura_media_type = $entry_details->mediaType;
  $node->kaltura_duration = $entry_details->duration;
  $node->kaltura_thumbnail_url = $entry_details->thumbnailUrl;
  $node->kaltura_partner_data = 'imported '. $type;
  //$node->kaltura_source = $entry_details['source'];
  //$node->kaltura_source_id = $entry_details['sourceId'];
  //$node->kaltura_source_link = $entry_details['sourceLink'];
  $node->kaltura_width = $entry_details->width;
  $node->kaltura_height = $entry_details->height;
  $node->kaltura_download_url = $entry_details->downloadUrl;
  //$node->kaltura_media_date = $entry_details['mediaDate'];
  $node->kaltura_views = $entry_details->views;
  $node->kaltura_plays = $entry_details->plays;
  $node->kaltura_votes = $entry_details->votes;
  $node->kaltura_rank = $entry_details->rank;
  $node->kaltura_total_rank = $entry_details->totalRank;
  node_save($node);
  module_invoke('node_kaltura_'. $type, 'hook_insert', $node);
  drupal_set_message('Kaltura item imported: '. l($node->title, 'node/'. $node->nid) .' (id: '. $node->kaltura_entryId .')');
}

/*
 * helper function to provide form fields containing all entries for import
 * each entry as checkbox option
 */
function kaltura_create_entries_form($form) {
  $args = func_get_args();
  $entries_options = $args[1][0];
  $form['entries_to_import'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Choose the entries you want to import'),
    '#options' => $entries_options,
  );
  $form['import_now'] = array(
    '#type' => 'submit',
    '#title' => t('import selected'),
    '#value' => t('import selected'),
  );
  return $form;
}

/*
 * helper functuion to get all entries of partner from Kaltura's DB
 * TODO
 * CMAC
 * changes:
 * now retrieving all media entries - needs to import mix entries as well
 */
function kaltura_get_my_entries($filter, $type, $page_size = 30, $page = 1) {
  $isAdmin = 1;
  $session_user = KalturaHelpers::getSessionUser();
  $kaltura_client = KalturaHelpers::getKalturaClient($isAdmin); // this is admin task and should be able to import all entries in the account
  $pager_filter = new KalturaFilterPager();
  $pager_filter->pageIndex = $page;
  $pager_filter->pageSize = $page_size;
  //$result = $kaltura_client->listMyEntries($session_user, $filter, 0, $page_size, $page);
  switch($type){
    case KalturaEntryType::MIX:
      $result = $kaltura_client->mixing->listAction($filter,$pager_filter);
      break;
    case KalturaEntryType::MEDIA_CLIP:
      $filter->mediaTypeEqual = KalturaEntryType::MEDIA_CLIP;
      $result = $kaltura_client->media->listAction($filter,$pager_filter);
      break;
  }

  if ($result->totalCount > 0) {
    $count = $result->totalCount;
    //$each_page = $result['result']['page_size'];
    if (is_array($result->objects) && count($result->objects)) {
      foreach ($result->objects as $entry) {
        $returned_entries[$entry->id] = array(
                'id' => $entry->id,
                'name' => $entry->name,
                'type' => $entry->type,
        );
      }
    }
    return array($count, $returned_entries);
  }
  else {
    return array(0, array());
  }
}

/*
 * helper function that recieves a list of entries, and returns a new list
 * which does not contain entries that are already represented as nodes
 */
function kaltura_remove_existing_entries($entries) {
  $count = 0;
  $new_entries = array();
  if (is_array($entries) && count($entries)) {
    foreach ($entries as $entry) {
      $sel = 'SELECT COUNT(nid) n FROM {node_kaltura} WHERE kaltura_entryId = \'%s\'';
      $result = db_fetch_object(db_query($sel, $entry['id']));
      if ($result->n == 0) {
        $new_entries[] = $entry;
      }
      else {
        $count++;
      }
    }
  }
  return $new_entries;
}


/*
 * helper function that checks the drupal cron job status
 * cron job is required to collect statictics about kaltura items
 */
function kaltura_test_cron_job_status() {
  $cron_last = variable_get('cron_last', NULL);
  if (!$cron_last) {
    return array(
    FALSE,
      'cron has not run. If you don\'t configure cron job, statistics about kaltura items will not be updated.'
      );
  }
  else {
    return array(
    TRUE,
      'cron has run. Be sure that you have a cron job configured, so you don\'t need to run it manually.<br />'.
      'Statistics about kaltura items will only be updated at cron run.'
      );
  }
}

/*
 * helper function that tests the notification status of the module
 *
 * this function tries to close a loop with the kaltura sevrer in a "real-time" test.
 * The notification handler sets a variable in the DB before the this function run ends
 * therefore we need to use the kaltura_variable_get_real() function to check the variable status in the DB
 * and not in the $_GLOBALS which are set before this function even start
 * TODO
 * CMAC
 * not yet implemented
 */
function kaltura_test_notification_config() {
  return array(TRUE, 'Test call not yet implemented, sorry');
  if (kaltura_variable_get_real('kaltura_notification_status', 0) == 0) {
    $session_user = KalturaHelpers::getSessionUser();
    $kaltura_client = KalturaHelpers::getKalturaClient();
    $result = $kaltura_client->testNotification($session_user);
    sleep(3);
  }
  if (kaltura_variable_get_real('kaltura_notification_status', 0) === 1) {
    return array(TRUE, 'Drupal server receives notification from Kaltura server.');
  }

  return array(FALSE, 'Drupal server fails to receive notifications from Kaltura. Please fix your configuration or disable server notifications.');
}

/*
 * replace default Drupal's variable_get() function with a "real-time" one
 *
 * This function checks a variable status in the DB and not in the $_GLOBALS
 *
 */
function kaltura_variable_get_real($var, $default) {
  $sql = 'SELECT * FROM {variable} WHERE name = :name';
  $result = db_query($sql, array('name' => $var));
  if (!$result) {
    return $default;
  }

  return unserialize($result->value);
}

/**
 * Used when SimpleXML is not available; just assume everything looks OK and hope for the best.
 */
function kaltura_alternate_xml_verification($filename) {
  return array(
    TRUE,
    t('crossdomain.xml is in place and seems to be OK')
  );
}

/**
 * helper function that checks if crossdomain.xml is in the right place
 */
function kaltura_test_crossdomain() {
  global $base_url ;
  $cd_req = drupal_http_request($base_url .'/crossdomain.xml');
  if ( $cd_req->code == '200' ) {
    return kaltura_parse_crossdomain($cd_req->data);
  }
  else {
    // fall back to fetching and retrieving file contents (should we
    // succeed if HTTP request has failed, though?
    $filename = $_SERVER['DOCUMENT_ROOT'] .'/crossdomain.xml';
    if ( $cd_xml = file_get_contents($filename) ) {
      return kaltura_parse_crossdomain($cd_xml) ;
    }
    else {
      return array(
        FALSE,
        t('crossdomain.xml file could not be found in your site\'s root directory') .' ['. $filename .']' .
        t('Please read ') . l('here', 'admin/help/kaltura')
      );
    }
  }
}

/**
 * helper function that checks if crossdomain.xml says the right things
 */
function kaltura_parse_crossdomain($crossdomain_xml) {
  if (!function_exists('simplexml_load_string')) {
    return kaltura_alternate_xml_verification($filename);
  }
  $cd_xml_content = simplexml_load_string($crossdomain_xml);
  foreach ($cd_xml_content->children() as $child) {
    $atts = $child->attributes();
    if ($atts['domain'] == '*' || $atts['domain'] == '*.kaltura.com') {
      $kaltura_ok = TRUE;
    }
    if ($atts['headers'] == '*' && ($atts['domain'] == '*' || $atts['domain'] == '*.kaltura.com')) {
      $headers_ok = TRUE;
    }
  }
  if ($kaltura_ok && $headers_ok) {
    return array(
      TRUE,
      t('crossdomain.xml is in place and seems to be OK')
    );
  }
  else {
    return array(
      FALSE,
      t('crossdomain.xml is in place, but it is probably not configured properly') .'<br />'.
      t('Please read ') . l('here', 'admin/help/kaltura')
    );
  }
}

/*
 * helper function that updates partner notification settings in kaltura's server
 *
 * CMAC
 * changes
 */
function kaltura_update_kaltura_partner($notifications_config = 2, $notification_url = '') {
  $partner = new KalturaPartner();
  $partner->allowMultiNotification = '1';
  $partner->notify = 1;
  $partner->notificationsConfig = '*='. $notifications_config;
//  $partner->id = variable_get('kaltura_partner_id', '');

  $url = url('kaltura/notification_handler', array('absolute' => TRUE));
  $notification_url = ($notification_url == '')? $url: $notification_url;
  $partner->notificationUrl = $notification_url;
  $session_user = KalturaHelpers::getSessionUser();
  $kaltura_client = KalturaHelpers::getKalturaClient(1);
   $cms_pass = variable_get('kaltura_partner_cms_password', '');

  if ($kaltura_client) {
    $result = $kaltura_client->partner->update($partner, $cms_pass);
    //$result = $kaltura_client->updatePartner($session_user, $partner);
  }
  return $result;
}

/*
 * TODO
 * Settings form in URL admin/settings/kaltura/general/notifications
 */
function kaltura_notifications_settings_form() {
  global $base_url;
  $notification_types = array(
  //'1' => 'only server side notifications',
    '2' => 'only client side notifications',
    '3' => 'both server & client notifications'
    );
    if (isset($_REQUEST['kaltura_notification_type']) && $_REQUEST['kaltura_partner_url2']) {
      // TODO: update partner settings with notification type
      $url2 = $_REQUEST['kaltura_partner_url2'] .'/?q=kaltura/notification_handler';
      if ($_REQUEST['kaltura_notification_type'] == 1) {
      $not_type = 3;
      }
      else {
      $not_type = 2;
      }
      $result = kaltura_update_kaltura_partner($not_type, $url2);
      if ($_REQUEST['last_notification_url'] != $_REQUEST['kaltura_partner_url2'] || $not_type == 3) {
      variable_set('kaltura_notification_status', 0);
      list($status, $text) = kaltura_test_notification_config();
      if ($status == FALSE) {
        drupal_set_message(t('Notification test failed. The URL you specified is not valid. Please make sure it is correct and save the settings again.'), 'error');
      }
      else {
        drupal_set_message(t('Notification test was successful.'));
      }
      }
      if (!$result->id) {
      drupal_set_message('Updating kaltura\'s server faild.<br />'. $result['error']['code'] .': '. $result['error']['desc'], 'error');
      }
      else {
      drupal_set_message('Updating kaltura\'s server was successful '. l($url2, $url2));
      }
    }
    $notification_type_description = t('Enabling server notifications allows the Kaltura video platform to update your Drupal server about the status of '.
    'the media content. This is especially useful when uploading content in various formats that needs to be transcoded. '.
    'In order to support server side notifications, your server must be publicly available on the internet.');
    $form['kaltura_notification_type'] = array(
    '#type' => 'checkbox',
    '#options' => 'Enable server side notifications',
    '#description' => $notification_type_description,
    '#title' => t('Enable server side notifications'),
    '#default_value' => variable_get('kaltura_notification_type', 0),
    );

    $not_url = (variable_get('kaltura_partner_url2', '') != '')? variable_get('kaltura_partner_url2', ''): $base_url;
    $form['kaltura_partner_url2'] = array(
    '#type' => 'textfield',
    '#title' => 'Notification URL',
    '#description' => t('Your drupal base URL.') .' /?q=kaltura/notification_handler '. t('will be added to this base URL.'),
    '#default_value' => $not_url,
    );

    $form['last_notification_url'] = array(
    '#type' => 'hidden',
    '#value' => variable_get('kaltura_partner_url2', ''),
    );
    return $form;
    return system_settings_form($form);
}

/*
 * helper function that tries to open session with the kaltura server
 *
 * used to verify partner details, and display errors if there are
 *
 * CMAC
 * changed
 * added new variable_get statements required for the api calls
 * updated KalturaClient creation process
 * changed API calls
 *
 * TODO check correct format for error checking of result
 */
function kaltura_check_server_status($admin) {
  //get the variables we will need to start the session
/*  $partner_id = variable_get('kaltura_partner_id', '');
  $secret = variable_get('kaltura_secret', '');
  $admin_secret = variable_get('kaltura_admin_secret', '');



  $session_user = KalturaHelpers::getSessionUser();
  $config = KalturaHelpers::getServiceConfiguration();
  $config->partnerId = $partner_id;
  $kaltura_client = new KalturaClient();
  $kaltura_client->setConfig($config);




  if ($admin == 1) {
    $result = $kaltura_client->session->start($partner_id, $secret, $session_user->screenName);
  }
  else {
    $result = $kaltura_client->session->start($partner_id, $admin_secret, $session_user->screenName, KalturaSessionType::ADMIN);
  }
*/

  try
  {
    $result = KalturaHelpers::getKalturaClient($admin);
    $kaltura_status = TRUE;
    $kaltura_error = "";
  }
  catch(Exception $ex)
  {
    $kaltura_status = FALSE;
    $kaltura_error = $ex->getMessage();
  }

  /*if ($result[]) {
    $kaltura_status = FALSE;
    $kaltura_error = $result['error'][0]["code"] .' '. $result['error'][0]["desc"];
    }
    else {*/
  //  }
  return array($kaltura_status, $kaltura_error);
}

/*
 * helper function to generate a cms password
 *
 * cms is the "administration panel" for kaltura's partners in kaltura's website
 */
function kaltura_generate_cms_password() {
  $pass = '';
  $pass = md5(time() . $_SERVER['REMOTE_ADDR']);
  $pass = drupal_substr($pass, 0, 8);
  return $pass;
}
