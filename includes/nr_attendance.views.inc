<?php
/**
 * Implements hook_views_data().
 */
function nr_attendance_views_data()
{

    $data = array();

    $data['nr_attendance']['table']['group'] = t('Node Registration Attendance');

    $data['nr_attendance']['table']['base'] = array(
        'title' => t('Node Registration Attendance'),
        'help' => t('Contains records of Node Registration Attendance(on nr_registration db table).'),
    );
		$data['nr_attendance']['attendance_id'] = array(
		    'title' => t('Attendance ID'),
		    'help' => t('The nr_attendance ID.'),
		    'field' => array(
		        'handler' => 'views_handler_field_numeric',
		    ),
		    'sort' => array(
		        'handler' => 'views_handler_sort',
		    ),
		    'filter' => array(
		        'handler' => 'views_handler_filter_numeric',
		    ),
		);
		$data['nr_attendance']['node_type'] = array(
		    'title' => t('Node Type'),
		    'help' => t('The node type of node_registration is associated with.'),
		    'field' => array(
		        'handler' => 'views_handler_field',
		    ),
		    'sort' => array(
		        'handler' => 'views_handler_sort',
		    ),
		    'filter' => array(
		        'handler' => 'views_handler_filter_string',
		    ),
		);
		$data['nr_attendance']['nid'] = array(
		    'title' => t('Node ID'),
		    'help' => t('The node ID of node_registration is associated with.'),
		    'field' => array(
		        'handler' => 'views_handler_field_node',
		    ),
		    'sort' => array(
		        'handler' => 'views_handler_sort',
		    ),
		    'filter' => array(
		        'handler' => 'views_handler_filter_numeric',
		    ),
		    'relationship' => array(
        'base' => 'node',
        'field' => 'nid', // of nr_registration
        'handler' => 'views_handler_relationship',
        'label' => t('Node'),
		    ),
		    'argument' => array(
		        'handler' => 'views_handler_argument_node_nid',
		        'numeric' => TRUE,
		        'validate type' => 'nid',
		    ),
		);
		$data['nr_attendance']['registration_id'] = array(
		    'title' => t('Registration ID'),
		    'help' => t('The node_registration ID of this nr_attendance is associated with.'),
		    'field' => array(
		        'handler' => 'views_handler_field',
		    ),
		    'sort' => array(
		        'handler' => 'views_handler_sort',
		    ),
		    'filter' => array(
		        'handler' => 'views_handler_filter_numeric',
		    ),
		    'relationship' => array(
        'base' => 'node_registration',
        'field' => 'registration_id', // of nr_registration
        'handler' => 'views_handler_relationship',
        'label' => t('Node Registration'),
		    ),
		    'argument' => array(
		        'handler' => 'views_handler_argument',
		        'numeric' => TRUE,
		        'validate type' => 'registration_id',
		    ),
		);
		$data['nr_attendance']['uid'] = array(
		    'title' => t('User ID'),
		    'help' => t('The user ID of User entity is associated with.'),
		    'field' => array(
		        'handler' => 'views_handler_field_user',
		    ),
		    'sort' => array(
		        'handler' => 'views_handler_sort',
		    ),
		    'filter' => array(
		        'handler' => 'views_handler_filter_numeric',
		    ),
		    'relationship' => array(
        'base' => 'user',
        'field' => 'uid', // of nr_registration
        'handler' => 'views_handler_relationship',
        'label' => t('User'),
		    ),
		    'argument' => array(
		        'handler' => 'views_handler_argument',
		        'numeric' => TRUE,
		        'validate type' => 'uid',
		    ),
		);

		$data['nr_attendance']['user_name'] = array(
		    'title' => t('Registree User Name'),
		    'help' => t('The user name of registree.'),
		    'field' => array(
		        'handler' => 'views_handler_field',
		    ),
		    'sort' => array(
		        'handler' => 'views_handler_sort',
		    ),
		    'filter' => array(
		        'handler' => 'views_handler_filter_string',
		    ),
		);
		$data['nr_attendance']['user_email'] = array(
		    'title' => t('Registree User Email'),
		    'help' => t('The registree user email address.'),
		    'field' => array(
		        'handler' => 'views_handler_field',
		    ),
		    'sort' => array(
		        'handler' => 'views_handler_sort',
		    ),
		    'filter' => array(
		        'handler' => 'views_handler_filter_string',
		    ),
		);
		$data['nr_attendance']['trainer_id'] = array(
		    'title' => t('Trainer User ID'),
		    'help' => t('The trainer user ID, the training/tutoring node author.'),
		    'field' => array(
		        'handler' => 'views_handler_field_numeric',
		    ),
		    'sort' => array(
		        'handler' => 'views_handler_sort',
		    ),
		    'filter' => array(
		        'handler' => 'views_handler_filter_numeric',
		    ),
		);
		$data['nr_attendance']['trainer_name'] = array(
		    'title' => t('Trainer User Name'),
		    'help' => t('The trainer user name, the training/tutoring node author.'),
		    'field' => array(
		        'handler' => 'views_handler_field',
		    ),
		    'sort' => array(
		        'handler' => 'views_handler_sort',
		    ),
		    'filter' => array(
		        'handler' => 'views_handler_filter_string',
		    ),
		);
		$data['nr_attendance']['updater_id'] = array(
		    'title' => t('Updater User ID'),
		    'help' => t('The last administrator user ID who updated this row.'),
		    'field' => array(
		        'handler' => 'views_handler_field_numeric',
		    ),
		    'sort' => array(
		        'handler' => 'views_handler_sort',
		    ),
		    'filter' => array(
		        'handler' => 'views_handler_filter_numeric',
		    ),
		);
		$data['nr_attendance']['updater_name'] = array(
		    'title' => t('Updater User Name'),
		    'help' => t('The last administrator user name who updated this row.'),
		    'field' => array(
		        'handler' => 'views_handler_field',
		    ),
		    'sort' => array(
		        'handler' => 'views_handler_sort',
		    ),
		    'filter' => array(
		        'handler' => 'views_handler_filter_string',
		    ),
		);
		$data['nr_attendance']['created'] = array(
		    'title' => t('Created nr'),
		    'help' => t('The created date/time.'),
		    'field' => array(
		        'handler' => 'views_handler_field_date',
		    ),
		    'sort' => array(
		        'handler' => 'views_handler_sort_date',
		    ),
		    'filter' => array(
		        'handler' => 'date_views_filter_handler_simple',
		    ),
		);
		$data['nr_attendance']['cancelled'] = array(
		    'title' => t('Cancelled nr'),
		    'help' => t('The cancelled date/time.'),
		    'field' => array(
		        'handler' => 'views_handler_field_date',
		    ),
		    'sort' => array(
		        'handler' => 'views_handler_sort_date',
		    ),
		    'filter' => array(
		        'handler' => 'date_views_filter_handler_simple',
		        //'is date' => true,
		    ),
		);
		$data['nr_attendance']['attended'] = array(
		    'title' => t('Attended nr'),
		    'help' => t('The attended date/time.'),
		    'field' => array(
		        'handler' => 'views_handler_field_date',
		    ),
		    'sort' => array(
		        'handler' => 'views_handler_sort_date',
		    ),
		    'filter' => array(
		        'handler' => 'date_views_filter_handler_simple',
		    ),
		);
		$data['nr_attendance']['confirmed'] = array(
		    'title' => t('Confirmed nr'),
		    'help' => t('The confirmed date/time.'),
		    'field' => array(
		        'handler' => 'views_handler_field_date',
		    ),
		    'sort' => array(
		        'handler' => 'views_handler_sort_date',
		    ),
		    'filter' => array(
		        'handler' => 'date_views_filter_handler_simple',
		    ),
		);
		$data['nr_attendance']['bypassed'] = array(
		    'title' => t('Bypassed nr'),
		    'help' => t('The bypassed date/time.'),
		    'field' => array(
		        'handler' => 'views_handler_field_date',
		    ),
		    'sort' => array(
		        'handler' => 'views_handler_sort_date',
		    ),
		    'filter' => array(
		        'handler' => 'date_views_filter_handler_simple',
		    ),
		);
		$data['nr_attendance']['deleted'] = array(
		    'title' => t('Deleted nr'),
		    'help' => t('The deleted date/time.'),
		    'field' => array(
		        'handler' => 'views_handler_field_date',
		    ),
		    'sort' => array(
		        'handler' => 'views_handler_sort_date',
		    ),
		    'filter' => array(
		        'handler' => 'date_views_filter_handler_simple',
		    ),
		);
		$data['nr_attendance']['updated'] = array(
		    'title' => t('Updated nr'),
		    'help' => t('The last updated date/time.'),
		    'field' => array(
		        'handler' => 'views_handler_field_date',
		    ),
		    'sort' => array(
		        'handler' => 'views_handler_sort_date',
		    ),
		    'filter' => array(
		        'handler' => 'date_views_filter_handler_simple',
		    ),
		);
		$data['nr_attendance']['updated_str'] = array(
		    'title' => t('Updated nr str'),
		    'help' => t('The last updated date/time as string.'),
		    'field' => array(
		        'handler' => 'views_handler_field_date',
		    ),
		    'sort' => array(
		        'handler' => 'views_handler_sort_date',
		    ),
		    'filter' => array(
		        'handler' => 'date_views_filter_handler_simple',
		    ),
		);
		$data['nr_attendance']['note'] = array(
		    'title' => t('Note nr'),
		    'help' => t('The attendance note, mainly used for trainer report.'),
		    'field' => array(
		        'handler' => 'views_handler_field',
		    ),
		    'sort' => array(
		        'handler' => 'views_handler_sort',
		    ),
		    'filter' => array(
		        'handler' => 'views_handler_filter_string',
		    ),
		);


    return $data;

}

/**
 * Implements hook_views_data_alter().
 *
 * Create 'reversed' relationships eg. from node to registration(s)
 */
function nr_attendance_views_data_alter(&$data) {

  // Node to Registration Attendance.
  $data['node']['nr_attendance_rel'] = array(
    'group' => t('Node registration'),
    'title' => t('Registrations Attendance nr'),
    'help' => t('Relate from node to it\'s registrations attendance.'),
    'relationship' => array(
      'label' => t('Registrations Attendance nr'),
      'base' => 'nr_attendance',
      'base field' => 'nid',
      'field' => 'nid',
      'handler' => 'views_handler_relationship',
    ),
  );
}