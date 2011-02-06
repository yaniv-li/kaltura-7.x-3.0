// $Id: README.txt,v 1.2.2.3 2010/07/07 09:34:53 kaltura Exp $

Kaltura All in One Video Module – add full video capabilities to your Drupal site with our open source video module
------------------------

To install, place the entire Kaltura folder into your modules directory.

Move the crossdomain.xml file (supplied with the module) to the Root directory of your domain
If your domain is domain.com.
This file should be accessible in http://domain.com/crossdomain.xml

In your Drupal admin console, go to Administer -> Site building -> Modules and enable the Kaltura module and one or
more (recommended to enable all) additional modules:

- Kaltura Media Node
- Kaltura Media Remix
- Kaltura Views
- Kaltura as CCK field
- Kaltura Video Comments

Now go to Administer -> Site configuration -> Kaltura -> Server Integration Settings. 

Register to the Kaltura Partner Program using the registration form. Your Kaltura partner details will be automatically
saved into the module configuration, and will be sent to you in an email as well.
According to the modules you enabled, you can now start working with the Kaltura All in One Video Module 
to create nodes, and enable your site with full video and rich media capabilities.

It is recommended to configure a cron job for your Drupal site, since
some statistics data about the media in your site is updated by using a hook_cron.
Some of the views provided with the module rely on those statistics.

Further documentation about the Kaltura module for Drupal can be found here:
http://corp.kaltura.com/wiki/index.php/Kaltura-drupal-module



Maintainers
-----------
The Kaltura All in One Video Module was developed by: Kaltura, Inc.
www.kaltura.com

Current maintainers:
Kaltura, Inc. www.kaltura.com

