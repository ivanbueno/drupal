<?php

class Hackathon {

  var $current_user = NULL;

  var $event_node = NULL;
  var $event_node_wrapper = NULL;
  var $project_node_current_wrapper = NULL;
  var $date_current = NULL;
  var $projects = NULL;
  var $participants = NULL;
  var $participants_count = NULL;
  var $deadline = NULL;
  var $project_count = NULL;
  var $member_limit = NULL;

  var $is_deadline = FALSE;
  var $is_idea_submission_deadline = FALSE;
  var $is_owner = NULL;
  var $allow_join = NULL;
  var $is_organizer = NULL;

  function __construct($event) {
    global $user;
    $this->current_user = $user;

    $this->event_node = $event;
    $this->event_node_wrapper = entity_metadata_wrapper('node', $event);

    // Process dates
    $now = new DateTime("now", new DateTimeZone('UTC'));
    $this->date_current = $now->getTimestamp();

    $this->deadline = new DateTime($event->field_submission_deadline[LANGUAGE_NONE][0]['value'],
      new DateTimeZone('UTC'));
    $date_idea_submission_deadline = new DateTime($event->field_idea_submission_deadline[LANGUAGE_NONE][0]['value'],
      new DateTimeZone('UTC'));
    $this->is_deadline = $this->date_current >= $this->deadline->getTimestamp();
    $this->is_idea_submission_deadline = $this->date_current >= $date_idea_submission_deadline->getTimestamp();

    $this->member_limit = $this->event_node_wrapper->field_event_member_limit->value();
  }

  // -----------

  function get_event_nid() {
    return $this->event_node_wrapper->nid->value();
  }

  function get_date_start() {
    $date_days = field_view_field('node', $this->event_node, 'field_duration', array('settings' => array('format_type' => 'normal'), 'label' => 'hidden'));
    $date_start = new DateTime($date_days['#items'][0]['value'], new DateTimeZone($date_days['#items'][0]['timezone']));
    return $date_start;
  }

  function process_projects($skip_project_data = FALSE) {
    $this->projects = array();
    $this->is_owner = FALSE;
    $this->allow_join = TRUE;
    $this->project_count = 0;
    $this->participants = array();

    foreach ($this->event_node_wrapper->field_projects->getIterator() as $delta => $project_wrapper) {
      $project_value = $project_wrapper->value();
      if (empty($project_value)) {
        continue;
      }

      $roles_needed = array();
      if (isset($project_wrapper->field_project_roles)) {
        foreach ($project_wrapper->field_project_roles->getIterator() as $role_delta => $role) {
          $roles_needed[] = $role->name->value();
        }
      }

      $this->participants[] = $project_wrapper->author->mail->value();

      $project_members = $roster = array();
      $project_members_count = 0;

      $roster[] = array(
        'uid' => $project_wrapper->author->uid->value(),
        'mail' => $project_wrapper->author->mail->value(),
        'sso' => $project_wrapper->author->name->value(),
        'role' => 'Project Champion',
      );

      if (isset($project_wrapper->field_project_members)) {
        foreach ($project_wrapper->field_project_members->getIterator() as $project_member_delta => $project_member) {
          $project_members[] = $project_member->field_member_name->uid->value();
          $this->participants[] = $project_member->field_member_name->mail->value();

          $roster[] = array(
            'uid' => $project_member->field_member_name->uid->value(),
            'mail' => $project_member->field_member_name->mail->value(),
            'sso' => $project_member->field_member_name->name->value(),
            'role' => $project_member->field_member_role->name->value(),
          );
        }
        $project_members_count = $project_wrapper->field_project_members->count();
      }

      $this->project_count++;
      $this->participants_count += $project_members_count;

      $is_project_member = in_array($this->current_user->uid, $project_members);
      if ($is_project_member) {
        $this->allow_join = FALSE;
      }

      $is_project_owner = $this->current_user->uid == $project_wrapper->value()->uid;
      if ($is_project_owner && !$project_wrapper->field_project_declined->value()) {
        $this->is_owner = TRUE;
      }

      $is_member_limit_reached = $project_members_count >= $this->member_limit;

      if (!$skip_project_data) {
        $is_project_open = $project_wrapper->field_project_accept_members->value();
        $project_status = $is_project_open && $roles_needed ? 'open' : 'full';

        if ($is_member_limit_reached) {
          $project_status = 'full';
        }

        if ($project_wrapper->field_project_completed->value() == 1) {
          $project_status = 'complete';
        }

        if ($project_wrapper->field_project_declined->value() == 1) {
          $project_status = 'declined';
        }

        $project_node = $project_wrapper->value();
        $field_logo_style = array(
          'label' => 'hidden',
          //'settings' => array('image_style' => 'adaptive_100x100')
        );
        $project_logo_field = field_view_field('node', $project_node, 'field_logo', $field_logo_style);
        $project_logo = render($project_logo_field);
        $project_tags = field_view_field('node', $project_node, 'field_tags', 'default');

        $this->projects[$project_status][$project_wrapper->nid->value()] = array(
          'project_logo' => $project_logo,
          'field_pitch' => $project_wrapper->field_pitch->value(),
          'field_description' => $project_wrapper->field_description->value(),
          'title' => $project_wrapper->title->value(),
          'path' => '/' . drupal_get_path_alias('node/' . $project_wrapper->nid->value()),
          'field_project_roles' => $roles_needed,
          'members_count' => $project_members_count,
          'is_member' => $is_project_member,
          'is_owner' => $is_project_owner,
          'is_project_open' => $is_project_open,
          'field_tags' => render($project_tags),
          'comment_count' => $project_node->comment_count,
          'roster' => $roster,
        );
      }
    }
  }

  function get_projects() {
    if (is_null($this->projects)) {
      $this->process_projects();
    }
    return $this->projects;
  }

  function get_project_count() {
    if (is_null($this->projects)) {
      $this->process_projects(TRUE);
    }

    return $this->project_count;
  }

  function get_participants_count() {
    if (is_null($this->projects)) {
      $this->process_projects(TRUE);
    }

    return $this->participants_count;
  }

  function get_participants() {
    if (is_null($this->projects)) {
      $this->process_projects(TRUE);
    }

    return array_unique($this->participants);
  }

  public function get_event_title() {
    return $this->event_node_wrapper->title->value();
  }

  public function get_deadline() {
    return $this->deadline;
  }

  public function get_member_limit() {
    return $this->member_limit;
  }

  // -----------

  function is_deadline() {
    return $this->is_deadline;
  }

  function is_idea_submission_deadline() {
    return $this->is_idea_submission_deadline;
  }

  function allow_project_pitch() {
    return $this->event_node_wrapper->field_event_settings_pitch->value();
  }

  function allow_project_create() {
    if (is_null($this->is_owner) || is_null($this->allow_join)) {
      $this->process_projects(TRUE);
    }
    return !$this->is_idea_submission_deadline && $this->allow_join && !$this->is_owner;
  }

  function allow_project_join() {
    if (is_null($this->is_owner) || is_null($this->allow_join)) {
      $this->process_projects(TRUE);
    }
    return !$this->is_deadline && $this->allow_join && !$this->is_owner;
  }

  function is_organizer() {
    $is_organizer = FALSE;
    foreach ($this->event_node_wrapper->field_organizers->getIterator() as $delta => $project_organizer) {
      if ($this->current_user->uid == $project_organizer->uid->value()) {
        $is_organizer = TRUE;
        break;
      }
    }
    $this->is_organizer = $is_organizer;
    return $this->is_organizer;
  }
  // -----------

  public static function get_projects_by_user($uid) {
    $result = db_query("SELECT fpm.entity_id, n.title, ttd.name
      FROM field_data_field_project_members fpm
      LEFT JOIN node n ON n.nid = fpm.entity_id
      LEFT JOIN field_data_field_member_name fmn ON fpm.field_project_members_value = fmn.entity_id
      LEFT JOIN field_data_field_project_roles fpr ON fpm.entity_id = fpr.entity_id
      LEFT JOIN taxonomy_term_data ttd ON fpr.field_project_roles_tid = ttd.tid
      WHERE fmn.field_member_name_target_id = :uid;", array(
      ':uid' => $uid
    ));

    $projects = array();

    $project_ids = array();
    if ($result) {
      foreach ($result as $record) {
        $project_ids[] = $record->entity_id;

        $projects[$record->entity_id] = array(
          'title' => $record->title,
          'role' => $record->name,
        );
      }
    }

    if (count($project_ids)) {
      $event_results = db_query("SELECT fp.field_projects_target_id as pid, n.title, n.nid
      from field_data_field_projects fp
      LEFT JOIN node n
      ON n.nid = fp.entity_id
      WHERE fp.field_projects_target_id
      IN (:nids)", array(':nids' => $project_ids));
      if ($event_results) {
        foreach ($event_results as $event) {
          $projects[$event->pid]['event'] = array(
            'title' => $event->title,
            'nid' => $event->nid,
          );
        }
      }
    }

    return $projects;
  }

}

class HackathonProject extends Hackathon {
  var $project_wrapper = NULL;
  var $project_members = NULL;

  public function __construct($project_wrapper) {
    $this->project_wrapper = $project_wrapper;

    // Event
    if (isset($project_wrapper->field_parent_event)
      && $project_wrapper->field_parent_event->getIdentifier()) {
      $event_nid = $project_wrapper->field_parent_event->nid->value();
    }
    else {
      $query = new EntityFieldQuery();
      $query->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'event')
        ->propertyCondition('status', NODE_PUBLISHED)
        ->fieldCondition('field_projects', 'target_id', $project_wrapper->nid->value())
        ->range(0, 1);
      $result = $query->execute();
      if (isset($result['node'])) {
        $event_nids = array_keys($result['node']);
        $event_nid = array_pop($event_nids);
      }
    }
    $this->event_nid = $event_nid;
    $event = node_load($event_nid);

    parent::__construct($event);
  }

  public function get_roles_needed() {
    $roles_needed = array();
    foreach ($this->project_wrapper->field_project_roles->getIterator() as $role_delta => $role) {
      $roles_needed[$role->tid->value()] = $role->name->value();
    }
    return $roles_needed;
  }

  public function get_members() {
    $project_members = array();
    foreach ($this->project_wrapper->field_project_members->getIterator() as $project_member_delta => $project_member) {
      $pm_name = trim($project_member->field_member_name->field_first_name->value() . ' ' . $project_member->field_member_name->field_last_name->value());
      if (empty($pm_name)) {
        $pm_name = $project_member->field_member_name->name->value();
      }

      $sso = $project_member->field_member_name->name->value();
      $project_members[$project_member->field_member_name->uid->value()] = array(
        'role' => $project_member->field_member_role->name->value(),
        'image' => '<img src="https://supportcentral.gecdn.com/images/person/temp/' . $sso . '.jpg" />',
        'mail' => $project_member->field_member_name->mail->value(),
        'name' => $pm_name,
        'delta' => $project_member_delta,
        'sso' => $sso,
      );
    }
    $this->project_members = $project_members;
    return $project_members;
  }

  public function is_project_owner() {
    global $user;
    return $this->project_wrapper->value()->uid == $user->uid;
  }

  public function is_project_declined() {
    return $this->project_wrapper->field_project_declined->value();
  }

  public function is_project_completed() {
    return $this->project_wrapper->field_project_completed->value();
  }

  public function is_project_member($uid) {
    if (is_null($this->project_members)) {
      $members = $this->get_members();
    }
    return array_key_exists($uid, $this->project_members);
  }

  public function is_project_open() {
    return $this->project_wrapper->field_project_accept_members->value();
  }

  public function search_project_member($uid) {
    if ($this->is_project_member($uid)) {
      return $this->project_members[$uid];
    }
    else {
      return FALSE;
    }
  }
 }