<?php
/**
 * @file
 * Attendance functions
 *
 * Update the table data
 */
function _create_attendance($registration){

	$anow = _get_time_now();
	$registee = user_load($registration->uid);
	$username = $registee->name;

	$attendanceId = db_insert('nr_attendance')
					->fields(array('node_type' => $registration->node_type,
													'registration_id' => $registration->registration_id,
													'nid' => $registration->nid,
													'user_email' => $registration->email,
													'uid' => $registration->uid,
													'user_name' => $username,
													'trainer_id' => $registration->node->uid,
													'trainer_name' => $registration->node->name,
													'created' => $registration->created,
													//'updated' => $registration->created,
													'updated_str' => $anow['string'],
												))->execute();

	// To store registee's phone number on the profile page
	$obj = entity_metadata_wrapper('user', $registee);
	// You can now access the fields without knowing the language
	$obj->field_phone_number = $registration->field_phone_number['und'][0]['value'];
	// save changes
	$obj->save();

}

function _update_attendance($registration){

	global $user;	$anow = _get_time_now();

	$result = db_update('nr_attendance')
						->condition('registration_id', $registration->registration_id, '=')
						->fields(array('node_type' => $registration->node_type,
														'trainer_id' => $registration->node->uid,
														'trainer_name' => $registration->node->name,
														//'confirmed' => $registration->verified,
														'attended' => ($registration->attended != 0) ? $anow['timestamp'] : 0,
														'cancelled' => ($registration->cancelled != 0) ? $anow['timestamp'] : 0,
														'updater_id' => $user->uid,
														'updater_name' => $user->name,
														'updated' => $anow['timestamp'],
														'updated_str' => $anow['string'],
													))
						->execute();
}

function _update_attendance_report($registration){

	global $user;	$anow = _get_time_now();

	$result = db_update('nr_attendance')
						->condition('registration_id', $registration->registration_id, '=')
						->fields(array(
											'note' => $registration->note,
											'updater_id' => $user->uid,
											'updater_name' => $user->name,
											'updated' => $anow['timestamp'],
											'updated_str' => $anow['string'],
										)
									)
						->execute();
	return $result;
}

function _delete_attendance($registration){

	global $user; $anow = _get_time_now();

	$result = db_update('nr_attendance')
						->condition('registration_id', $registration->registration_id, '=')
						->fields(array('deleted' => $anow['timestamp'],
														'updater_id' => $user->uid,
														'updater_name' => $user->name,
														'updated' => $anow['timestamp'],
														'updated_str' => $anow['string'],
													))
						->execute();
}

/*
 * Get user attendances by a user ID
 * DEPRECATED~!
 *
function _get_attendance_by_uid($uid, $case=''){

	switch($case){
		case 'unattended':
			// any trainings have not been cancelled, attended, confirmed, bypassed, nomaly any upcoming trainings
			$result = db_select('nr_attendance', 'nr')->fields('nr')->condition('uid', $uid, '=')->condition('cancelled', 0, '=')->condition('attended', 0, '=')->condition('confirmed', 0, '=')->condition('bypassed', 0, '=')->execute()->fetchAll();
			break;
		case 'missed':
			// any trainings have not been attended but marked as confirmed missed by trainer, this means the user did not show up for the training
			$result = db_select('nr_attendance', 'nr')->fields('nr')->condition('uid', $uid, '=')->condition('cancelled', 0, '=')->condition('attended', 0, '=')->condition('confirmed', 0, '>')->condition('bypassed', 0, '=')->execute()->fetchAll();
			break;
		case 'attended':
			// any trainings have been attended
			$result = db_select('nr_attendance', 'nr')->fields('nr')->condition('uid', $uid, '=')->condition('cancelled', 0, '=')->condition('attended', 0, '>')->execute()->fetchAll();
			//$OR = db_or()->condition('attended', 0, '>')->condition('confirmed', 0, '>')->condition('bypassed', 0, '>');
			//$result = db_select('nr_attendance', 'nr')->fields('nr')->condition('uid', $uid, '=')->condition('cancelled', 0, '=')->condition($OR)->execute()->fetchAll();
			break;
		case 'cancelled':
			// any trainings have been cancelled
			$result = db_select('nr_attendance', 'nr')->fields('nr')->condition('uid', $uid, '=')->condition('cancelled', 0, '>')->execute()->fetchAll();
			break;
		default:
			$result = db_select('nr_attendance', 'nr')->fields('nr')->condition('uid', $uid, '=')->execute()->fetchAll();
	}

	return $result;
}/**/

/*
 * Get current time as Unix timestamp and String
 */
function _get_time_now(){

	return array('timestamp' => time(), 'string' => date('Ymd-H:i:s'));

}

/*
 * Return attendances by a node ID
 * @description: return value is an array of Non-cancelled attendance
 */
function _get_attendance_by_nid($nid, $registrationtype = '', $all = false){
	$query = db_select('nr_attendance', 'nr');
		//$query->innerjoin('field_data_field_date', 'f', 'nr.nid = f.entity_id');
		$query->fields('nr')
					//->condition('f.field_date_value', $now, '>') // greater than (Upcoming)
					->condition('nr.nid', $nid, '=');

		// if $all is false then only grab non-canceled attendance
		if(!$all){
			$query->condition('nr.cancelled', 0, '=')->condition('nr.bypassed', 0, '=');
		}

		// if the training type is passed use it to filter the query result.
		if($registrationtype != '') $query->condition('nr.node_type', $registrationtype, '=');

	$result = $query->execute()->fetchAll();

	return $result;
}

/*
 * Return ATTENDED attendances by a user ID
 * ! STATIC FIELD TYPE IS USED : $query->innerjoin('field_data_field_date', 'nf', 'nr.nid = nf.entity_id')
 * ! this should be resolved replaced with a value from the valiable table later!
 * @description: $registrationtype parameter: t_class, t_individual, t_core
 */
function _get_attended_attendance_by_uid($uid, $registrationtype, $weekcount = 1){
	$now = time();
	//$now = strtotime("3 March 2015"); //for debugging only
	$query = db_select('nr_attendance', 'nr');
	$query->innerjoin('field_data_field_date', 'f', 'nr.nid = f.entity_id');
	$query->fields('nr')
				->condition('nr.uid', $uid, '=')
				->condition('nr.bypassed', 0, '=')
				->condition('nr.attended', 0, '>') // make sure to grab the one has been marked as attended
				->condition('nr.cancelled', 0, '=');

/* EXAMPLE TO APPLY "OR" QUERY
	$db_or = db_or(); // OR condition
	$db_or->condition('nr.bypassed', 0, '>'); // make sure to grab the one has been bypassed.
	$db_or->condition('nr.attended', 0, '>'); // make sure to grab the one has been marked as attended
	$query->condition($db_or);
*/

	// to get attended attendance in same week for core music tutoring or individual training
	if($registrationtype == 't_core' || $registrationtype == 't_individual'){

		// Due to PHP "this week" expression on date function bug(using on Sunday), if today is Sunday, we have to check with monday last week to compare instead monday this week, because checking "monday this week" on sunday(with today) will get next monday(tomorrow).
		$thismonday = (date('w', time()) == 0 ? strtotime('monday last week') : strtotime('monday this week')); //$nextmonday = strtotime('monday next week');
		//dsm(date('Y m d', $thismonday));
		//dsm(date('Y m d', time()));

		$query->condition('f.field_date_value', $thismonday, '>'); // greater than beginning of this monday
		$query->condition('f.field_date_value', $now, '<'); // less than (Past)
	}
	else
	{
		$query->condition('f.field_date_value', $now, '<'); // less than (Past)
	}

	// the training type has to be passed use it to filter the query result.
	$query->condition('nr.node_type', $registrationtype, '=');

	$result = $query->execute()->fetchAll();

	return $result;

}


/*
 * Return attendances by a trainer/tutor ID
 * ! STATIC FIELD TYPE IS USED : $query->innerjoin('field_data_field_date', 'nf', 'nr.nid = nf.entity_id')
 * ! this should be resolved replaced with a value from the valiable table later!
 * @description: $registrationtype parameter: t_class, t_individual, t_core
 */
function _get_unchecked_attendance_by_tid($tid, $registrationtype = ''){
	$now = time();
	//$now = strtotime("23 March 2015"); //for debugging only

	// the query to get all attendances (Not canlled and not bypassed) belong to the trainer
	$query = db_select('nr_attendance', 'nr');
		$query->innerjoin('field_data_field_date', 'f', 'nr.nid = f.entity_id');
		$query->fields('nr')
					->condition('f.field_date_value', $now, '<') // less than (Past)
					->condition('nr.trainer_id', $tid, '=')
					//->condition('nr.attended', 0, '=')
					//->condition('nr.confirmed', 0, '=')
					->condition('nr.bypassed', 0, '=')
					->condition('nr.cancelled', 0, '=');

		// if the training type is passed use it to filter the query result.
		if($registrationtype != '') $query->condition('nr.node_type', $registrationtype, '=');

	$results = $query->execute()->fetchAll();

	if(!empty($results)){

		$missedReportNodes = array(); $missedAttendanceNodes = array();

		foreach($results as $result){

			$nid = $result->nid;
			$node = node_load($nid);
			$endDate = $node->field_date['und'][0]['value2'];

			// check if the session is ended and there is no comment for the session.
			if($endDate < time() && !$node->comment_count){
				// collect the node id and date with proper display format
				$missedReportNodes[$nid] = format_date($node->field_date['und'][0]['value'], $type = 'long', $format = '', $node->field_date['und'][0]['timezone']);
			}

			// check if the session is NOT confirmed.
			if(!$result->confirmed){
				// collect the node id and date with proper display format
				$missedAttendanceNodes[$nid] = format_date($node->field_date['und'][0]['value'], $type = 'long', $format = '', $node->field_date['und'][0]['timezone']);
			}

		}

		$notice = '';

		// any missing session report
		if(!empty($missedReportNodes) && $registrationtype != "t_core"){
			$notice .= 'You have not completed your session reports. Click on the date to fill out any missing reports.<br />';
			foreach($missedReportNodes as $nid => $date){
				$notice .= '<a href="/node/'.$nid.'" target="_blank">'.$date.'</a><br />';
			}
			$notice .= '<br />';
		}

		// any missing session attendance
		if(!empty($missedAttendanceNodes)){
			$notice .= 'You have not submitted attendance for the sessions below. Click on the date to submit attendance.<br />';
			foreach($missedAttendanceNodes as $nid => $date){
				$notice .= '<a href="/node/'.$nid.'" target="_blank">'.$date.'</a><br />';
			}
			$notice .= '<br />';

		}


	}

	return $notice;

}

/*
 * this will be deprecated soon.
 */
function _display_trainer_message($account, $registrationtype = ''){

	//$userDisplayName = '<b>'._get_displayname($account).'</b>';

	$anyTrainings = _get_unchecked_attendance_by_tid($account->uid, $registrationtype);

	//$trainingType = array('t_class'=> 0, 't_individual'=> 0, 't_core'=> 0);
	$trainingTypes = array();
	foreach ($anyTrainings as $each){
		if(!isset($trainingTypes[$each->node_type])) $trainingTypes[$each->node_type] = 1;
	}

	if(!empty($trainingTypes)){
		$count = count($trainingTypes);
		//$message = ''.$userDisplayName.',';
		if($count == 0){
			//$message .= '<br />You don\'t have any upcoming registration,<br />you may check our Technology Training and Core Music Tutoring on top menu to add one.';
		}
		else{
			//$message .= 'You have NOT report the attendance for your past training(s), visit <a href="/manage/attendance/my-student">My Student page</a> to report them.';
			//$message .= 'You have NOT reported attendance for your past training(s), please check the list below and report them.';
		}

		//drupal_set_message(t($message), 'error');
	}

	return $anyTrainings;
}

function _display_missing_report_message($uid, $bundleType){

		switch($bundleType){
			case 't_core':

				// check if trainer missed any report and/or attendance
				$sessionReportNotice = _get_unchecked_attendance_by_tid($uid, $bundleType);
				if($sessionReportNotice){
					drupal_set_message($sessionReportNotice, 'error');
				}

			break;
			case 't_individual':

				// check if trainer missed any report and/or attendance
				$sessionReportNotice = _get_unchecked_attendance_by_tid($uid, $bundleType);
				if($sessionReportNotice){
					//drupal_set_message($sessionReportNotice.'You will not be able to add new '.$bundleName.' session until this issue is resolved.', 'error');
					drupal_set_message($sessionReportNotice.'You will not be able to ADD or EDIT '.$bundleName.' sessions until you complete all reports and all attendance has been submitted.', 'error');

					return true;
				}

			break;
		}

}

/*
 * Return UPCOMING attendances by a user ID
 * ! STATIC FIELD TYPE IS USED : $query->innerjoin('field_data_field_date', 'nf', 'nr.nid = nf.entity_id')
 * ! this should be resolved replaced with a value from the valiable table later!
 * @description: $registrationtype parameter: t_class, t_individual, t_core
 */
function _get_upcoming_attendance_by_uid($uid, $registrationtype = ''){
	$now = time();
	//$now = strtotime("23 March 2015"); //for debugging only
	$query = db_select('nr_attendance', 'nr');
	$query->innerjoin('field_data_field_date', 'f', 'nr.nid = f.entity_id');
	$query->fields('nr')
				->condition('f.field_date_value', $now, '>') // greater than (Upcoming)
				->condition('nr.uid', $uid, '=')
				->condition('nr.cancelled', 0, '=');

		// if the training type is passed use it to filter the query result.
		if($registrationtype != '') $query->condition('nr.node_type', $registrationtype, '=');

	$result = $query->execute()->fetchAll();

	return $result;

}
function _display_upcoming_message($account, $currentPage = ''){

	$userDisplayName = '<b>'._get_displayname($account).'</b>';

	$anyTrainings = _get_upcoming_attendance_by_uid($account->uid);
	//$trainingType = array('t_class'=> 0, 't_individual'=> 0, 't_core'=> 0);
	$trainingTypes = array();
	foreach ($anyTrainings as $each){
		if(!isset($trainingTypes[$each->node_type])) $trainingTypes[$each->node_type] = 1;
	}
	if(!empty($trainingTypes)){
		$count = count($trainingTypes);
		$message = ''; //'Welcome back '.$userDisplayName.',';
		if($count == 0){
			$message .= 'Welcome back '.$userDisplayName.',<br />You are not registered for any upcoming sessions, visit our Technology Training or Core Music Tutoring pages if you would like to sign up.';
		}
		//if($count == 1){
			//$message .= '<br />Visit <a href="/manage/attendance/my">My Registrations</a> to view your upcoming sessions.';
		//}
		else{
			// disable this notice since all user will be redirected to My Attendance page on login.
			//$message .= '<br />Visit <a href="/manage/attendance/my">My Registrations</a> to view your upcoming sessions.';
		}

		drupal_set_message(t($message), 'status');
	}

	return $anyTrainings;
}
/*
 * Return MISSED attendances by a user ID
 * ! STATIC FIELD TYPE IS USED : $query->innerjoin('field_data_field_date', 'nf', 'nr.nid = nf.entity_id')
 * ! this should be resolved replaced with a value from the valiable table later!
 * @description: $registrationtype parameter: t_class, t_individual, t_core
 */
function _get_missed_attendance_by_uid($uid, $registrationtype = ''){
	$now = time();
	//$now = strtotime("3 March 2015"); //for debugging only
	$query = db_select('nr_attendance', 'nr');
		$query->innerjoin('field_data_field_date', 'f', 'nr.nid = f.entity_id');
		$query->fields('nr')
					->condition('nr.uid', $uid, '=')
					->condition('nr.bypassed', 0, '=')
					->condition('nr.attended', 0, '=')
					->condition('nr.cancelled', 0, '=');

		// to ignore any missed Core music tutoring happened on last week
		if($registrationtype == 't_core'){
			$thismonday = strtotime('monday this week'); //$nextmonday = strtotime('monday next week');
			$query->condition('f.field_date_value', $thismonday, '>'); // greater than beginning of this monday
			$query->condition('f.field_date_value', $now, '<'); // less than (Past)
		}
		else
		{
			$query->condition('f.field_date_value', $now, '<'); // less than (Past)
		}
		// if the training type is passed use it to filter the query result.
		if($registrationtype != '') $query->condition('nr.node_type', $registrationtype, '=');

	$result = $query->execute()->fetchAll();

	return $result;

}
function _display_missed_message($account, $currentPage = ''){

	$userDisplayName = '<b>'._get_displayname($account).'</b>';

	$anyTrainings = _get_missed_attendance_by_uid($account->uid);

	$trainingTypes = array();
	foreach ($anyTrainings as $each){

		if(!isset($trainingTypes[$each->node_type])) $trainingTypes[$each->node_type] = 1;

	}

	if(!empty($trainingTypes)){

		if(isset($trainingTypes['t_class'])){

			// find out if there is any NOT CONFIRMED session
			foreach ($anyTrainings as $each){

				// check if class is actually on ABSENT status, then display message
				if($each->node_type == 't_class' && $each->updated > 0 && $each->attended == 0){

					drupal_set_message(t('Our records indicate that you('.$userDisplayName.') did not attend a class you signed up for. If you cannot attend a class, please consider cancelling next time in order to free up seats for other students.'), 'warning');

					break;
				}
			}
		}

		if(isset($trainingTypes['t_individual'])){

			if($currentPage == 'manage/attendance/my'){

				drupal_set_message(t('Our records indicate that you('.$userDisplayName.') did not attend the Peer Training session you signed up for.<br />Verify the list below and contact Learning Center staff to resolve this issue, otherwise you will not be able to sign up for any future training.'), 'error');

			}else{

				drupal_set_message(t('Our records indicate that you('.$userDisplayName.') did not attend the Peer Training session you signed up for. Visit <a href="/manage/attendance/my">My Registrations</a> to verify and contact Learning Center staff to resolve this issue, otherwise you will not be able to sign up for any future training.'), 'error');

			}

		}

		if(isset($trainingTypes['t_core'])){
			if($currentPage == 'manage/attendance/my'){
				drupal_set_message(t('Our records indicate that you('.$userDisplayName.') did not attend the Core Music Tutoring session you signed up for.<br />Verify the list below and contact Learning Center staff to resolve this issue, otherwise you will not be able to sign up for any future tutoring.'), 'error');
			}else{
				drupal_set_message(t('Our records indicate that you('.$userDisplayName.') did not attend the Core Music Tutoring session you signed up for. Visit <a href="/manage/attendance/my">My Registrations</a>  to verify and contact Learning Center staff to resolve this issue, otherwise you will not be able to sign up for any future tutoring.'), 'error');
			}
		}
	}

	//return $anyTrainings;
}


/*
 * views bulk operation action function
 * @description: setting nr_attendance properties
 */
function _attendance_action_update($delta, &$nrregistration, $bool=true){
	$anow = _get_time_now();
	switch($delta){
		case 'confirm':
			$nrregistration->confirmed = $bool == true ? $anow['timestamp'] : 0;
			break;
		case 'bypass':
			$nrregistration->bypassed = $bool == true ? $anow['timestamp'] : 0;
			break;
		case 'attend':
			$nrregistration->attended = $bool == true ? $anow['timestamp'] : 0;
			break;
	}
	_set_updater($nrregistration, $anow);
	return;
}
/*
 * views bulk operation action function
 * @description: setting nr_attendance updater properties
 */
function _set_updater(&$nrregistration, $anow){
	global $user;
	$nrregistration->updated = $anow['timestamp'];
	$nrregistration->updated_str = $anow['string'];
	$nrregistration->updater_id = $user->uid;
	$nrregistration->updater_name = $user->name;
}
/*
 * To save user attendance status when user login
 * Deprecated for now.
 */
function _update_user_attendance_statue($user){

	// this is the result of any trainings that are not cancelled, attended, confirmed and bypassed.
	// without any human(student, trainer) error, this should all be upcoming.
	$result = _get_attendance_by_uid($user->uid, 'unattended');
	$anow = _get_time_now();

	// to filter all training into two arrays for upcoming and missed.
	$upcoming = array('t_class'=>array(), 't_individual'=>array(), 't_core'=>array());
	$missed = array('t_class'=>array(), 't_individual'=>array(), 't_core'=>array());
	foreach($result as $each){
		$nid = $each->nid;
		$node = node_load($nid);
		$value = field_get_items('node', $node, 'field_date')[0]['value'];
		if($value > $anow['timestamp']){
			switch($each->node_type){
				case 't_class':
					$upcoming['t_class'][$nid]['node'] = $node;
					$upcoming['t_class'][$nid]['registration'] = $each;
					break;
				case 't_individual':
					$upcoming['t_individual'][$nid]['node'] = $node;
					$upcoming['t_individual'][$nid]['registration'] = $each;
					break;
				case 't_core':
					$upcoming['t_core'][$nid]['node'] = $node;
					$upcoming['t_core'][$nid]['registration'] = $each;
					break;
			}
		}
		else{
			switch($each->node_type){
				case 't_class':
					$missed['t_class'][$nid]['node'] = $node;
					$missed['t_class'][$nid]['registration'] = $each;
					break;
				case 't_individual':
					$missed['t_individual'][$nid]['node'] = $node;
					$missed['t_individual'][$nid]['registration'] = $each;
					break;
				case 't_core':
					$missed['t_core'][$nid]['node'] = $node;
					$missed['t_core'][$nid]['registration'] = $each;
					break;
			}
		}
	}
	$status = 1000;

	if(!empty($upcoming['t_class'])){
		$class = 'You have upcoming <b>class training(s)</b>, would you like to see what they are? <a class="btn btn-success" href="/manage/attendance/my">YES</a>';
		drupal_set_message($class, 'statue', false);
		$status = $status + 100; // value 100 stands for upcoming Class training
	}
	else{
		$class = 'No upcoming class training.';
		drupal_set_message($class, 'statue', false);
	}
	if(!empty($upcoming['t_individual'])){
		$individual = 'You have a upcoming <b>individual training</b>, please visit <a href="/manage/attendance/my">your registration page</a> to remind yourself.';
		drupal_set_message($individual, 'statue', false);
		$status = $status + 10; // value 10 stands for upcoming Individual training
	}
	else{
		$individual = 'No upcoming individulal training.';
		drupal_set_message($individual, 'statue', false);
	}
	if(!empty($upcoming['t_core'])){
		$core = 'You have a upcoming <b>core tutoring</b>, please visit <a href="/manage/attendance/my">your registration page</a> to remind yourself.';
		drupal_set_message($core, 'statue', false);
		$status = $status + 1; // value 1 stands for upcoming Core tutoring
	}
	else{
		$core = 'No upcoming core tutoring.';
		drupal_set_message($core, 'statue', false);
	}

	_set_user_nr_attendance_field($user, $status);

	return $status;
}
function _set_user_nr_attendance_field($user, $status, $statusCurrent = 0){
//$account = user_load($uid); // Loading account
	$edit = array(
							  'field_nr_attendance' => array(
							    'und' => array(
							      0 => array(
							        'value' => $status,
							      ),
							    ),
							  ),
							);
	$update = user_save($user, $edit);
}
function _get_user_nr_attendance_field($uid){
	$user = user_load($uid); // Loading account
	$value = field_get_items('user', $user, 'field_nr_attendance')['und'][0]['value'];

	return $value;
}


function _get_displayname($account){

	//if(is_object($account)) $account = user_load($account->uid);
	$field = field_get_items('user', $account, 'field_display_name');
	$value = field_view_value('user', $account, 'field_display_name', $field[0]);

/*
	if($account->uid == 104){
		dsm($variables);
		dsm($field);
	}
*/

	return render($value);
}

function _html_bootstrap_panel($title, $body, $css = 'panel panel-info'){
	return '<div class="'.$css.'">
						<div class="panel-heading"><h3 class="panel-title">'.$title.'</h3></div>
	 					<div class="panel-body">'.$body.'</div>
	 				</div>';
}
function _html_bootstrap_alert($message, $type='danger'){
// $type key = success, info, warning, danger
	switch($type){
		case 'danger':
			return '<div class="nr-attendance alert alert-danger" role="alert">'.$message.'</div>';
		case 'warning':
			return '<div class="nr-attendance alert alert-warning" role="alert">'.$message.'</div>';
		case 'info':
			return '<div class="nr-attendance alert alert-info" role="alert">'.$message.'</div>';
		case 'success':
			return '<div class="nr-attendance alert alert-success" role="alert">'.$message.'</div>';
	}

}
function _block_registration_form($entity){
	$entity->content['registration_form'] = array(
							'#type' => 'container',
							'#attributes' => array('class' => array('nr-attendance', 'nr-attendance-registration-form-blocked')),
							'content' => array(
								'#markup' => _html_bootstrap_alert('REGISTRATION BLOCKED', 'danger'),
								),
							'#access' => true,
						);
	return;
}

function _get_unregistrable_reason($reason, $typeName, $bundleMachineName, $returnUrl = '/', $sessionEntity = null){
		// Add reason for unregistrable.
    switch($reason){
	    case 'date':

				return _html_bootstrap_panel('Session unavailable.', 'This '.$typeName.' has already occured.<br /><a class="btn btn-primary btn-sm unregistrable" href="'.$returnUrl.'">Back To the '.$typeName.' list</a>', 'panel panel-warning');

	    case 'registered':

	    	if($sessionEntity){
	    		$gcal = _nr_attendance_add2gcal($sessionEntity->field_date, $sessionEntity->title.' : '.$typeName.' @ Berklee Learning Center', '', 'Learning Center, 150 Massachusetts Ave, Boston, MA', 'Learning Center @ Berklee College of Music');

	    	}

				return _html_bootstrap_panel('Registration complete.', 'You have successfully signed up this '.$typeName.', visit <a class="btn btn-primary btn-sm unregistrable" href="/manage/attendance/my">My Registrations</a> if you need to cancel.<br />'.$gcal, 'panel panel-info');

	    case 'capacity':
				return _html_bootstrap_panel($bundleMachineName == 't_class' ? 'No seats are currently available' : 'Session unavailable', ',<br />'.$bundleMachineName == 't_class' ? 'Please check back later to see if a spot has become available or contact the trainer for other options' : 'This '.$typeName.' is already booked by another student, please select another session.<br /><a class="btn btn-primary btn-sm unregistrable" href="'.$returnUrl.'">Back To '.$typeName.' list</a>', 'panel panel-warning');

	    case 'disabled':
	    case 'access':
	    	$dest = drupal_get_destination();
	    	if($GLOBALS['user']->uid == 0) return _html_bootstrap_panel('Please log in to proceed', 'Please log in above to proceed.', 'panel panel-warning');
	    	//if($GLOBALS['user']->uid == 0) return _html_bootstrap_panel('Registration Access Denied', 'Please log in to proceed.<br /><a class="btn btn-primary btn-sm unregistrable" href="/user/login?destination='.$dest['destination'].'">Go To Login page</a>', 'panel panel-warning');
				return _html_bootstrap_panel('Registration Access Denied', '<br />This registration is closed.', 'panel panel-danger');
			default:
				return '';
    }

}

/*
 * @description: this will apply to all training/tutoring node and verify the registration status.
 * @Todo: use the user field created by this module (field_nr_attendance) to update it's status so less query can be used.
 */
function _get_current_user_status($entity, $bundleMachineName, $typeName, $user, $reason){

//$missed = _get_missed_attendance_by_uid($uid, $bundleMachineName);
//global $user; $uid = $user->uid; $displayname = _get_displayname(user_load($uid));
		switch($bundleMachineName){
///// Class training	/////////////////////////////////////////////////////////////////
			case 't_class':
				$uid = $user->uid;
				// check if the user missed any past training
				$missed = _get_missed_attendance_by_uid($uid, $bundleMachineName);
				if(!empty($missed)){
					//$displayname = _get_displayname(user_load($uid));
					//return _html_bootstrap_panel('Past Class Training Missed', 'You have not been attended some of the past trainings you registered. You can still register for this class training but please make sure to cancel beforehand if you are going to miss it.', 'panel panel-warning');
					return '';
				}

				return _get_unregistrable_reason($reason, $typeName, $bundleMachineName, '/technology-training/finder?type=t_class', $entity);

///// Core Music Tutoring ////////////////////////////////////////////////////////////
			case 't_core':
				$uid = $user->uid;
				$displayname = _get_displayname(user_load($uid));

				///////////////////////////////////////////////////////////////////
				// check if the user is the author
				if(_is_user_author($user, $entity)){
					if(isset($entity->content['registration_form'])) _block_registration_form($entity);
					return _html_bootstrap_panel('This session was created by you', 'You can not register for your own tutoring session.', 'panel panel-warning');
				}
				///////////////////////////////////////////////////////////////////
				///////////////////////////////////////////////////////////////////
				// check if the current user registered any future tutoring
				$upcoming = _get_upcoming_attendance_by_uid($uid, $bundleMachineName);
				if(!empty($upcoming)){
					// block registration form
					if(isset($entity->content['registration_form'])) _block_registration_form($entity);
					// check if this tutoring is same as what user registered.
					if($upcoming[0]->nid == $entity->nid){
						return _html_bootstrap_panel('Registration complete', 'You have successfully signed up for this tutoring session, visit <a href="/manage/attendance/my">My Registrations</a> if you need to cancel.', 'panel panel-info');
					}
					else{
						//$node = node_load($upcoming[0]->nid);
						//$registeredTraining = '<a href="/'.drupal_get_path_alias('node/'.$node->nid).'">Currently enrolled tutoring: '.t($node->title).'</a>';
						return _html_bootstrap_panel('You have an upcoming tutoring session.', $displayname.',<br />You may sign up for another session after you attend or cancel the one below.', 'panel panel-warning');
					}
				}
				///////////////////////////////////////////////////////////////////
				///////////////////////////////////////////////////////////////////
				// check if there is any missed tutoring
				$missed = _get_missed_attendance_by_uid($uid, $bundleMachineName);
				if(!empty($missed)){
					// block registration form
					if(isset($entity->content['registration_form'])) _block_registration_form($entity);
					return _html_bootstrap_panel('Tutoring session missed', 'You did not attend a tutoring session you registered for.<br />Contact Learning Center staff to resolve this issue and restore access.', $css = 'panel panel-danger');
				}
				///////////////////////////////////////////////////////////////////
				///////////////////////////////////////////////////////////////////
				// check if user attended tutoring this week
				$attendanceThisWeek = _get_attended_attendance_by_uid($uid, $bundleMachineName, 1);
				if(!empty($attendanceThisWeek)){
					// maximum allowance per week == 3 times
					$thisweekCount = count($attendanceThisWeek); $maximum = 3;
					if($thisweekCount >= $maximum){
							// block registration form
						if(isset($entity->content['registration_form'])) _block_registration_form($entity);
						return _html_bootstrap_panel('Maximum session limit', 'Sorry, you can only register for '.$maximum.' tutoring session per week. You have already taken the maximum number of sessions this week, please try next week.<br />', $css = 'panel panel-danger');

					}
					else{
						return _html_bootstrap_panel('Maximum of '.$maximum.' tutoring sessions per week', 'So far, you have taken '.$thisweekCount.' session'.($thisweekCount == 1 ? '': 's').', please be aware that you can take up to 3 tutoring sessions per week.<br />', $css = 'panel panel-warning');
					}

				}
				///////////////////////////////////////////////////////////////////


				return _get_unregistrable_reason($reason, $typeName, $bundleMachineName, '/core-music-tutoring/upcoming', $entity);

///// Individual training ////////////////////////////////////////////////////////////
			case 't_individual':
				$uid = $user->uid;
				$displayname = _get_displayname(user_load($uid));
				////////////////////////////////////////////////////////////////////
				// check if the user is the author
				if(_is_user_author($user, $entity)){

					// block registration form
					if(isset($entity->content['registration_form'])) _block_registration_form($entity);
					return _html_bootstrap_panel('This training was created by you', 'You can not register your own tutoring.', 'panel panel-warning');
				}
				///////////////////////////////////////////////////////////////////
				///////////////////////////////////////////////////////////////////
				// check if the current user registered any future tutoring
				$upcoming = _get_upcoming_attendance_by_uid($uid, $bundleMachineName);
				if(!empty($upcoming)){
					// block registration form
					if(isset($entity->content['registration_form'])) _block_registration_form($entity);
					// check if this tutoring is same as what user registered.
					if($upcoming[0]->nid == $entity->nid){
						return _html_bootstrap_panel('Registered by you', 'This training is registered by you, visit <a href="/manage/attendance/my">My Registrations</a> if you need to cancel.', 'panel panel-info');
					}
					else{

						//$node = node_load($upcoming[0]->nid);
						//$registeredTraining = '<a href="/'.drupal_get_path_alias('node/'.$node->nid).'">Currently enrolled tutoring: '.t($node->title).'</a>';
						return _html_bootstrap_panel('You have an upcoming peer training', $displayname.',<br />You cannot register for more than one peer training at a time. You may sign up for another session after you attend or cancel the one below.', 'panel panel-warning');
					}
				}
				///////////////////////////////////////////////////////////////////
				///////////////////////////////////////////////////////////////////
				// check if there is any missed tutoring
				$missed = _get_missed_attendance_by_uid($uid, $bundleMachineName);
				if(!empty($missed)){
					if(isset($entity->content['registration_form'])) _block_registration_form($entity);
					return _html_bootstrap_panel('Peer training missed', 'You did not attend or cancel a past peer training for which you were registered.<br />Contact Learning Center staff to restore your access to peer training.', $css = 'panel panel-danger');
				}
				///////////////////////////////////////////////////////////////////
				///////////////////////////////////////////////////////////////////
				// check if user attended tutoring this week
				$attendanceThisWeek = _get_attended_attendance_by_uid($uid, $bundleMachineName, 1);
				if(!empty($attendanceThisWeek)){

					// maximum allowance per week == 1 time for individual training
					$thisweekCount = count($attendanceThisWeek); $maximum = 1;
					if($thisweekCount >= $maximum){
//// line for week check statement by jsong, disabled for now
						// also check if this session is same week as user's previous session
						if(_nr_attendance_compare_weeknumber($entity->field_date['und'][0]['value'])){
							// block registration form
							if(isset($entity->content['registration_form'])) _block_registration_form($entity);
								return _html_bootstrap_panel('Maximum weekly peer training limit', 'Sorry, you may only register for '.$maximum.' peer training per week. Our records indicate you have already attended training this week. Please register instead for a session taking place next week.<br />', $css = 'panel panel-danger');
						}
					}
					else{
						//return _html_bootstrap_panel('Maximum of '.$maximum.' training per week', 'So far, you have taken '.$thisweekCount.' training'.$thisweekCount == 1 ? '': 's'.', please be aware of that you can take up to 3 tutoring per week.<br />', $css = 'panel panel-warning');
					}

				}
				///////////////////////////////////////////////////////////////////


				return _get_unregistrable_reason($reason, $typeName, $bundleMachineName, '/technology-training/finder', $entity);
		}
}
function _is_user_author($user, $entity){
	return $user->uid == $entity->uid;
}

/**
 * @description: automatically set endtime for the date field of node that registration is attached to.
 */
function _nr_attendance_field_date_validate($form, &$form_state) {
	$t_type = isset($form['type']['#value']) ? $form['type']['#value'] : false;
	if(isset($form_state['values']['field_date']['und'][0]['value']) && $t_type){
		$starttime = $form_state['values']['field_date']['und'][0]['value'];
		switch($t_type){
			case 't_individual':
				$form_state['values']['field_date']['und'][0]['value2'] = strtotime('+1 hour', $starttime); // set endtime to + 1hour (+ 3600)
			break;
			case 't_core':
				$form_state['values']['field_date']['und'][0]['value2'] = strtotime('+30 minutes', $starttime); //set endtime to + 30minutes (+ 1800)
				//$form_state['values']['field_date']['und'][0]['value2'] = strtotime('+1 hour', $starttime); // set endtime to + 1hour (+ 1800)
			break;
			case 't_class':
				$form_state['values']['field_date']['und'][0]['value2'] = strtotime('+1 hour', $starttime); // set endtime to + 1hour (+ 7200)
			break;
		}
	}
}

/**
 * Validation handler for nr_attendance_form().
 */
function _nr_attendance_report_form_validate($form, &$form_state) {

	if(empty($form_state['values']['report'])){
		form_set_error('report', 'The field is empty.');
	}
	elseif($form_state['values']['report'] == $form_state['nr_attendance']->note){
		form_set_error('report', 'No change has been made, the content is identical as previous.');
	}
	else{
		$form_state['nr_attendance']->note = $form_state['values']['report'];
	}

}

/**
 * Submit handler for nr_attendance_form().
 */
function _nr_attendance_report_form_submit($form, &$form_state) {

  // Save the registration.
  $success = _update_attendance_report($form_state['nr_attendance']);
  if($success){
  	drupal_set_message(t('The report updated successfully.'), 'status');
  }
  else{
	  drupal_set_message(t('The report updated faild, please contact LC staff to report the error.'), 'error');
  }
}

/**
 * get the week number by date
 */
function _nr_attendance_get_weeknumber($date = 0){
	if(!$date) $date = time();
	return (int)date('W', $date);
}
/**
 * compare weeknumbers of two date
 */
function _nr_attendance_compare_weeknumber($date1, $date2=0){

	$week1 = _nr_attendance_get_weeknumber($date1);
	$week2 = _nr_attendance_get_weeknumber(($date2 == 0 ? time() : $date2));

	return ($week1 == $week2);
}

/************************************************************************************
 * display google calendar button for a registered session
 ************************************************************************************/
function _nr_attendance_add2gcal($sessionDate, $title = '', $description = '', $location = '150 Massachusetts Ave, Boston, MA', $name = 'Learning Center @ Berklee College of Music'){

	global $base_url;

	$sessionStart = $sessionDate['und'][0]['value'];
	$sessionEnd = $sessionDate['und'][0]['value2'];
	$googlecalendarBtn = '';
	$themeFullUrl = $base_url.'/'.path_to_theme();

	$nodeFullUrl = $base_url.'/'.drupal_lookup_path('alias',"node/".$node->nid);
	/* GOOGLE CALENDAR : ADD TO YOUR GOOGLE CALENDAR BUTTON EXAMPLE GET VALUE STRING
		<a href="http://www.google.com/calendar/event?
		&text=jsongwhatjsong
		&dates=20130101T170000Z/20130101T180000Z
		&details=jsongdescriptionjsongjsongdescriptionjsongjsongdescriptionjsongjsongdescriptionjsong
		&location=jsongwherejsong
		&trp=false
		&sprop=jsongwebsiteaddressjsong
		&sprop=name:jsongwebsitenamejsong
		" target="_blank"><img src="//www.google.com/calendar/images/ext/gc_button1.gif" border=0></a>
	 */
	// CODE BELOW IS TO GENERATE "ADD TO YOUR GOOGLE CALENDER BUTTON" BY USING NODE INFORMATION
	$googlelink_prefix = '<a class="btn btn-primary btn-sm" href="http://www.google.com/calendar/event?action=TEMPLATE';
	$g_text = '&text='.$title;
	// use gmdate() to convert to GMT because the google calendar will always accept it as GMT and convert it there again.
	$g_dates = '&dates='.(gmdate('Ymd\THis', $sessionStart)).'Z/'.(gmdate('Ymd\THis', $sessionEnd)) . 'Z';
	$g_details = '&details=To view more information, copy and paste this web address(https://learningcenter.berklee.edu' . request_uri() . ') to your browser address bar.' ; // new
	$g_location = '&location='.$location;
	$g_trp = '&trp=false';
	$g_sprop = '&sprop=' . $nodeFullUrl; // website address;
	$g_spropname = '&sprop=name:'.$name; // website name;
	$googlelink_suffix = '" target="_blank" >Add to Google calendar</a>';

	$googlecalendarBtn = '<div class="add2gcal clearfix">'.
													$googlelink_prefix.
													$g_text.
													$g_dates.
													$g_details.
													$g_location.
													$g_trp.
													$g_sprop.
													$g_spropname.
													$googlelink_suffix.
												'</div>';

/*
	global $user;
	if($user->uid == 112){
		dsm(date_default_timezone_get());
		dsm($sessionDate);
		dsm($googlecalendarBtn);
	}
*/

	return $googlecalendarBtn;

}





