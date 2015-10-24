================================================================================
ABOUT Genova
================================================================================
Genova is a Drush utility targeted on Drupal developers who regularly create
custom Drupal modules. It is supposed to automatize routine code writing such as
schema, hook implementations, drupal arrays, etc.

The basic architecture aims to create very flexible and robust utility the basic
element of which is a component. A component has these responsibilities and/or
operations:

- init process that if not provided by a caller it will prompt him for the
missing input
- input validation and sanitization, so that if it comes to the build process
all the needed data is provided
- make the process of generating the code as transactional as possible

This approach allows components being created by other components in a way that
if not all data is provided the component will prompt to add missing pieces.
Also only those components should be created that have all valid inputs, so you
do not get stuck in the middle of process with the need to remove invalid code
by hand.

Genova is meant to be tool for developers and therefore to be extensible. It
provides and API so that any other module can create own component generator by
extending basic GenComponent class and providing own code templates. This gives
it the real power where you can use genova to create own components and by doing
so increase your productivity to maximum.

================================================================================
FEATURES
================================================================================
Current implemented components are:

- Drupal module structure
- hooks: menu, theme, permission, install/uninstal, schema
- sources: css, js, PHP classes file
- external modules: mforms, sdbo

================================================================================
INSTALLATION AND CONFIGURATION
================================================================================
To use Genova you need to have properly installed and configured Drush.
Note that this module has been tested only on UNIX systems so there is no
guarantee it will run on Windows.

To verify if all is working properly enter following command:

> drush help gen-mc

If you get help output, genova is installed and Drush is aware of its commands.

================================================================================
BASIC USAGE
================================================================================
For usage of specific commands see Drush help:
> drush help gen-[mc, cc]

The general syntax for genova commands is as follows:
drush GENOVA_COMMAND MODULE_NAME COMPONENT_NAME

Examples
--------------------------------------------------------------------------------

Console output for creating module and css component:

> dev@ubuntu:/var/www/d7/sites/all/scaffolding$ drush gen-mc my_module --components=css,js
> +++ Entering module component. +++
> Enter module description: My module description.
> +++ Entering CSS component. +++
> Component CSS created.
> Component output directory: /var/www/d7/sites/all/scaffolding/my_module
> +++ Entering JS component. +++
> Component JS created.
> Component output directory: /var/www/d7/sites/all/scaffolding/my_module
> Module my_module created at /var/www/d7/sites/all/scaffolding/my_module.
> dev@ubuntu:/var/www/d7/sites/all/scaffolding$

Previous command created directory my_module with basic Drupal module files and
its entries. Moreover it created css dir with my_module.css file and added
include for this css file into .info file.


Console output for implementing hook_menu and adding a path with page:

> dev@ubuntu:/var/www/d7/sites/all/scaffolding$ drush gen-cc my_module menu
> +++ Entering menu item component. +++
> Menu item path: my-module/%node
> Menu item title: My title
> Menu item description [empty]:
> Page callback function name: my_module_page
> Page arguments as they will be passed into array without spaces (i.e. "op_1",0,1) [empty]: 1
> Access callback function name [empty]:
> Access arguments as they will be passed into array without spaces (i.e. "op_1",0,1) [empty]: access content
> Menu item type [MENU_LOCAL_TASK]:
> Menu item file [my_module.pages.inc]:
> Menu item weight [0]:
> Menu path my-module/%node added.
> Component output directory: /var/www/d7/sites/all/scaffolding/my_module
> dev@ubuntu:/var/www/d7/sites/all/scaffolding$

Previous command implemented hook_menu into my_module.module file and added
menu item with params as provided into command prompt. It also created
my_module.pages.inc file and in there implemented appropriate page callback
function.

================================================================================
Version 1.0 TODO
================================================================================
- genova_inject_code_snippet() - implement use of $scope
- genova_hook_exists() - the logic that determines if the hook exists needs to
be more accurate
- Support for YUM as an input way
- Implement all, or most used basic Drupal hooks and Drupal array structures

================================================================================
SPONSOR
================================================================================

This module is a product of blueMinds (http://blueminds.eu) web development
company.
