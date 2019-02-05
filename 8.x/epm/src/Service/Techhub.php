<?php

namespace Drupal\epm\Service;

use Drupal\Core\Database\Connection;
use Drupal\user\Entity\User;
use Drupal\Component\EventDispatcher\ContainerAwareEventDispatcher;
use Drupal\node\Entity\Node;
use Drupal\taxonomy\TermInterface;
use Drupal\Component\Utility\Random;
use Drupal\workflow\Entity\WorkflowTransition;
use Drupal\workflow\Entity\WorkflowTransitionInterface;

/**
 * Techhub class.
 */
class Techhub {

    /**
     * The database connection.
     *
     * @var \Drupal\Core\Database\Connection
     */
    protected $database;

    /**
     * @var ContainerAwareEventDispatcher
     */
    private $eventDispatcher;

    /**
     * Constructs Techhub object.
     *
     * @param Connection $database
     *   The database connection.
     */
    public function __construct(Connection $database, ContainerAwareEventDispatcher $eventDispatcher) {
        $this->database = $database;
        $this->eventDispatcher = $eventDispatcher;
    }

    /**
     * Get all the hubs the admin or agent has access to.
     *
     * @param $uid
     */
    public function getHubsByAdmin($uid) {
        $query = $this->database->query("SELECT ttf_data.tid, ttf_data.name, tth.parent_target_id 
                    FROM {taxonomy_term__field_hub_users} ttf_users
                    LEFT JOIN {taxonomy_term_field_data ttf_data} ON ttf_users.entity_id = ttf_data.tid
                    LEFT JOIN {taxonomy_term__parent tth} ON tth.entity_id = ttf_data.tid
                    WHERE ttf_users.field_hub_users_target_id = :uid", [':uid' => $uid]);
        $tids = $query->fetchAll();

        // Check if the agent is a member of multiple organizations
        $parent_tids = [];
        $parent_root_count = 0;
        foreach ($tids as $tid) {
            if (!$tid->parent_target_id) {
                $parent_root_count++;
            }
            else {
                $parent_tids[] = $tid->parent_target_id;
            }
        }
        $has_root_hub = count(array_unique($parent_tids)) + $parent_root_count > 1 ? TRUE : FALSE;

        $hubs = [];
        foreach ($tids as $hub) {
            if ($hub->parent_target_id) {
                $parent = \Drupal::entityTypeManager()->getStorage('taxonomy_term')->loadParents($hub->tid);
                $parent = reset($parent);
                $parent_name = $has_root_hub ? $parent->label() . ' > ' : '';
                $hubs[$hub->tid] = $parent_name . $hub->name;
            }
            else {
                $terms =\Drupal::entityTypeManager()->getStorage('taxonomy_term')->loadTree('hubs', $hub->tid);
                foreach ($terms as $term) {
                    $parent_name = $has_root_hub ? $hub->name . ' > ' : '';
                    $hubs[$term->tid] = $parent_name . $term->name;
                }
            }
        }
        return $hubs;
    }


    public function createTicket($hub, $customer_uid, $author_uid = 0, $state = NULL, $comment = '') {
        $node = Node::create([
            'type' => 'ticket',
            'field_ticket_customer' => $customer_uid,
            'field_ticket_hub' => $hub,
            'uid' => $author_uid,
        ]);
        $node->save();

        if ($state) {
            $entity = \Drupal::entityManager()->getStorage('node')->load($node->id());
            /* @var $transition WorkflowTransitionInterface */
            $transition = WorkflowTransition::create(['ticket_status_open', 'field_name' => 'field_ticket_status']);
            $transition->setTargetEntity($entity);
            $transition->setValues($state, $entity->getOwnerId(), \Drupal::time()->getRequestTime(), $comment, TRUE);
            $transition->force();
            $transition->executeAndUpdateEntity();
        }

        return $node;
    }

    public function access(TermInterface $hub, $account, $access_key = '', $strict = FALSE) {
        if ($hub->bundle() == 'hubs') {
            $current_hub_access_key = $hub->get('field_hub_access_key')->getString();
            $allowed_hubs = array_keys($this->getHubsByAdmin($account->id()));

            // If it's an exhibit, allow public
            if (!$strict && $hub->get('field_hub_exhibit')->getString()) {
                return TRUE;
            }

            /**
             * User can access the hub page if they are:
             * 1. superadmin
             * 2. has explicit permission in the term
             * 3. access key is supplied in the request
             */
            $account_roles = $account->getRoles();
            if (in_array('super_admin', $account_roles)
                || in_array($hub->id(), $allowed_hubs)
                || $account->hasPermission('techhub level 2')
                || (!empty($current_hub_access_key) && $access_key == $current_hub_access_key)
            ) {
                return TRUE;
            }

            // Check if the user has access to a Parent hub
            $parent = \Drupal::entityTypeManager()->getStorage('taxonomy_term')->loadParents($hub->id());
            $parent = reset($parent);
            if (!$parent) {
                $admins = $hub->field_hub_users->getValue();
                foreach ($admins as $admin) {
                    if ($admin['target_id'] == $account->id()) {
                        return TRUE;
                    }
                }
            }
        }

        return FALSE;
    }

    public function getResolutionTime($nid) {
        $query = $this->database->query("
            SELECT workflow_transition_history.hid AS hid, 
              workflow_transition_history.to_sid as state,
              workflow_transition_history.timestamp
            FROM {workflow_transition_history} workflow_transition_history
            WHERE (workflow_transition_history.entity_id = :nid )
            ORDER BY workflow_transition_history.hid ASC", [':nid' => $nid]);
        $workflow_transitions = $query->fetchAll();

        $created = $start = $closure = 0;
        if (count($workflow_transitions) > 1) {

            // Get first instance
            $created = reset($workflow_transitions);
            $created = $created->timestamp;

            // Get last instance
            $closure = end($workflow_transitions);
            $closure = in_array($closure->state, ['ticket_status_complete', 'ticket_status_defer']) ? $closure->timestamp : 0;

            foreach ($workflow_transitions as $transition) {
                if (!$start && $transition->state == 'ticket_status_in_progress') {
                    $start = $transition->timestamp;
                }
            }
        }

        $service = $closure - $start;
        if (empty($start)) {
            $service = $closure - $created;
        }

        return [
            'wait' => $start - $created  > -1 ? $start - $created : 0,
            'service' => $service > -1 ? $service : 0,
            'total' => $closure - $created > -1 ? $closure - $created : 0,
        ];

    }

    public function latestUpdatedTicketsCount($tid, $timestamp) {
        $query = $this->database->query(
            "
            SELECT count(node_field_data.nid) AS ticket_count
            FROM {node_field_data} node_field_data
            INNER JOIN {node__field_ticket_hub} node__field_ticket_hub ON node_field_data.nid = node__field_ticket_hub.entity_id AND node__field_ticket_hub.deleted = '0'
            WHERE ((node__field_ticket_hub.field_ticket_hub_target_id = :tid)) AND ((node_field_data.status = '1') AND (node_field_data.type IN ('ticket')) AND ((node_field_data.changed > :timestamp)))
            ",
            [':tid' => $tid, ':timestamp' => $timestamp]
        );
        return $query->fetchField();
    }

    public function getFeedback($tids, $timestamp = NULL) {
        if (!$timestamp) {
            $timestamp = strtotime("-7 days");
        }

        $query = $this->database->query(
            "
            SELECT ws.created, wsd_hub.value as hub, wsd_recommendation.value as recommendation, wsd_comments.value as comments 
            FROM {webform_submission_data} wsd_hub
            LEFT JOIN {webform_submission_data} wsd_recommendation ON wsd_hub.sid = wsd_recommendation.sid
            LEFT JOIN {webform_submission_data} wsd_comments ON wsd_hub.sid = wsd_comments.sid
            LEFT JOIN {webform_submission} ws ON wsd_hub.sid = ws.sid
            WHERE wsd_hub.webform_id = 'customer_service_feedback'
            AND wsd_hub.name = 'techhub_hub'
            AND wsd_hub.value IN (:tids[])
            AND wsd_recommendation.name = 'techhub_recommendation'
            AND wsd_comments.name = 'techhub_comments'
            AND ws.created > :timestamp
            ",
            [':tids[]' => $tids, ':timestamp' => $timestamp]
        );

        return $query->fetchAll();
    }

    public function getFeedbackNps($tids, $timestamp = NULL) {
        $feedbacks = $this->getFeedback($tids, $timestamp);

        $negative = $neutral = $positive = 0;
        foreach ($feedbacks as $fb) {
            if ($fb->recommendation < 7) {
                $negative++;
            }
            elseif ($fb->recommendation >= 7 && $fb->recommendation <= 8) {
                $neutral++;
            }
            elseif ($fb->recommendation > 8) {
                $positive++;
            }
        }

        return [
            'detractor' => $negative,
            'passive' => $neutral,
            'promoter' => $positive,
            'score' => ($negative + $neutral + $positive) > 0 ?
                (($positive - $negative) / ($negative + $neutral + $positive)) * 100
                : 0,
        ];
    }

    public function getTickets($tid, $timestamp = NULL) {
        if (!$timestamp) {
            $timestamp = strtotime("-7 days");
        }

        $query = $this->database->query("
            SELECT node_field_data.nid AS nid
            FROM {node_field_data} node_field_data
            INNER JOIN {node__field_ticket_status} node__field_ticket_status ON node_field_data.nid = node__field_ticket_status.entity_id
              AND node__field_ticket_status.deleted = '0'
            LEFT JOIN {node__field_ticket_hub} node__field_ticket_hub ON node_field_data.nid = node__field_ticket_hub.entity_id
              AND node__field_ticket_hub.deleted = '0'
            WHERE (((node__field_ticket_status.field_ticket_status_value = 'ticket_status_complete'))
              AND ((node__field_ticket_hub.field_ticket_hub_target_id = :tid )))
              AND ((node_field_data.status = '1')
              AND (node_field_data.type IN ('ticket'))
              AND ((node_field_data.created > :timestamp)))
            ",
            [':tid' => $tid, ':timestamp' => $timestamp]
        );

        return $query->fetchAll();
    }

    public function getStatusTickets($tids, $timestamp = NULL) {
        if (!$timestamp) {
            $timestamp = strtotime("-7 days");
        }

        $query = $this->database->query(
            "
            SELECT node__field_ticket_status.field_ticket_status_value AS status, COUNT(DISTINCT node_field_data.nid) AS total
            FROM {node_field_data} node_field_data
            INNER JOIN {node__field_ticket_status} node__field_ticket_status ON node_field_data.nid = node__field_ticket_status.entity_id AND node__field_ticket_status.deleted = '0'
            LEFT JOIN {node__field_ticket_hub} node__field_ticket_hub ON node_field_data.nid = node__field_ticket_hub.entity_id AND node__field_ticket_hub.deleted = '0'
            WHERE (((node__field_ticket_status.field_ticket_status_value IN('ticket_status_open', 'ticket_status_in_progress', 'ticket_status_complete', 'ticket_status_defer', 'ticket_status_archive'))) 
              AND ((node__field_ticket_hub.field_ticket_hub_target_id IN(:tids[]) ))) 
              AND ((node_field_data.status = '1') 
              AND (node_field_data.type IN ('ticket')) 
              AND ((node_field_data.created > :timestamp)))
            GROUP BY node__field_ticket_status.field_ticket_status_value
            ",
            [':tids[]' => $tids, ':timestamp' => $timestamp]
        );

        return $query->fetchAllKeyed();
    }

    public function getProductivity($tids, $timestamp = NULL) {
        $total_productivity = $total_tickets = $total_money = $total_wait = $total_service = 0;

        foreach ($tids as $tid) {
            $tickets = $this->getTickets($tid, $timestamp);
            $total_tid_tickets = count($tickets);
            $total_tickets += $total_tid_tickets;

            $all_times = $wait_times = $service_times = 0;
            foreach ($tickets as $t) {
                $ticket_service_time = $this->getResolutionTime($t->nid);
                $all_times += $ticket_service_time['total'];
                $wait_times += $ticket_service_time['wait'];
                $service_times += $ticket_service_time['service'];
            }
            $total_wait += $wait_times;
            $total_service += $service_times;

            $term = \Drupal::entityTypeManager()->getStorage('taxonomy_term')->load($tid);
            $typical_service_time = $term->field_hub_productivity_typical->value;
            $agent_hourly_wage = $term->field_hub_productivity_rate->value;
            $giveback = ($term->field_hub_productivity_giveback->value ?
                            $term->field_hub_productivity_giveback->value
                            : 100)
                        * .01;

            $productivity = ($typical_service_time * $total_tid_tickets * 60) - $all_times;
            $productivity = $productivity > 0 ? $productivity : 0;
            $productivity = $productivity * $giveback;

            $total_productivity += $productivity;
            $total_money += ($productivity / 3600) * $agent_hourly_wage;
        }

        return [
            'time' => \Drupal::service('date.formatter')->formatInterval($total_productivity, $granularity = 2),
            'money' => number_format(round($total_money, 2)),
            'wait' => $total_tickets ? \Drupal::service('date.formatter')->formatInterval($total_wait / $total_tickets, $granularity = 2) : 0,
            'service' => $total_tickets ? \Drupal::service('date.formatter')->formatInterval($total_service / $total_tickets, $granularity = 2) : 0,
        ];
    }

    public function saveUser($email, $first = NULL, $last = NULL, $role = NULL, $hub = NULL, $override = TRUE, $notify_user = TRUE, $data = array()) {
        $random = new Random();
        $language = \Drupal::languageManager()->getCurrentLanguage()->getId();
        $notify = FALSE;

        $user = user_load_by_mail($email);
        if (!$user) {
            $user = User::create();
            $user->setPassword(strtolower($random->name(20)));
            $user->enforceIsNew();
            $user->setEmail($email);
            $user->setUsername($email);
            $user->set('init', $email);
            $user->set('langcode', $language);
            $user->set('preferred_langcode', $language);
            $user->set('preferred_admin_langcode', $language);

            if (isset($data['title'])) {
                $user->set('field_user_title', $data['title']);
            }

            if (isset($data['company'])) {
                $user->set('field_user_company', $data['company']);
            }

            $user->activate();
            $notify = TRUE;
        }

        if (empty($user->get('field_user_first_name')->getValue()) || ($override && $first)) {
            $user->set("field_user_first_name", $first);
        }
        if (empty($user->get('field_user_last_name')->getValue()) || ($override && $last)) {
            $user->set("field_user_last_name", $last);
        }
        if ($role) {
            $user->addRole($role);
        }
        if ($hub) {
            $current_hubs = $user->get('field_user_hubs')->getValue();
            $current_hubs_flat = [];
            foreach ($current_hubs as $current_hub) {
                $current_hubs_flat[] = $current_hub['target_id'];
            }
            if (!in_array($hub, $current_hubs_flat)) {
                $user->field_user_hubs[] = $hub;
            }
        }

        $result = $user->save();
        if ($notify_user && $notify) {
            _user_mail_notify('register_admin_created', $user);
        }

        return $user;
    }

    public function prepareMail($op, $options) {
        $message = [
            'to' => $options['to'],
            'from' => isset($options['from']) && $options['from'] ?
                $options['from'] :
                \Drupal::config('system.site')->get('mail'),
            'subject' => isset($options['subject']) ? $options['subject'] : '',
            'message' => '',
            'langcode' => isset($options['langcode']) && $options['langcode'] ?
                $options['langcode'] : \Drupal::currentUser()->getPreferredLangcode(),
        ];
        switch ($op) {
            case 'exhibit_send_assets':
                $term = \Drupal::entityTypeManager()->getStorage('taxonomy_term')->load($options['hub']);

                $files = [];
                $assets = $term->get('field_hub_assets')->getValue();
                foreach ($assets as $asset) {
                    $file = \Drupal\file\Entity\File::load($asset['target_id']);
                    $files[] = $asset['description'] . ':<br>' . $file->url();
                }

                $message['from'] = $term->get('field_hub_exhibit_email_sender')->getString();
                $message['subject'] = $term->get('field_hub_exhibit_email_subject')->getString();
                $body = [
                    '#theme' => 'epm_techhub_mail',
                    '#text' => str_replace(
                        ':assets',
                        implode('<br><br>', $files),
                        t(_filter_autop($term->get('field_hub_exhibit_email_body')->getString()), [
                            ':first_name' => $options['first_name'],
                            ':last_name' => $options['last_name'],
                        ])
                    ),
                    '#tokens' => [
                        'hub' => $term->getName(),
                        'heading' => NULL,
                        'copyright' => '(c) ' . date('Y') . ' TechHub Spaces.',
                        'optin' => NULL,
                    ],
                ];
                $message['message'] = \Drupal::service('renderer')->render($body);
                break;
        }
        return $message;
    }

    public function sendMail($op, $options) {
        $mail = \Drupal::service('plugin.manager.mail')->mail(
            'epm',
            $op,
            $options['to'],
            $options['langcode'],
            $options,
            NULL,
            TRUE
        );
    }

    public static function parse_csv($str) {
        // match all the non-quoted text and one series of quoted text (or the end of the string)
        // each group of matches will be parsed with the callback, with $matches[1] containing all the non-quoted text,
        // and $matches[3] containing everything inside the quotes
        $str = preg_replace_callback('/([^"]*)("((""|[^"])*)"|$)/s', array(__CLASS__, 'parse_csv_quotes' ), $str);

        //remove the very last newline to prevent a 0-field array for the last line
        $str = preg_replace('/\n$/', '', $str);

        //split on LF and parse each line with a callback
        return array_map(array(__CLASS__, 'parse_csv_line' ), explode("\n", $str));
    }

    public static function parse_csv_quotes($matches) {
        // anything inside the quotes that might be used to split the string into lines and fields later,
        // needs to be quoted. The only character we can guarantee as safe to use, because it will never appear in the unquoted text, is a CR
        // So we're going to use CR as a marker to make escape sequences for CR, LF, Quotes, and Commas.
        $str = '';
        if (isset($matches[3])) {
            $str = str_replace("\r", "\rR", $matches[3]);
            $str = str_replace("\n", "\rN", $str);
            $str = str_replace('""', "\rQ", $str);
            $str = str_replace(',', "\rC", $str);
        }

        // The unquoted text is where commas and newlines are allowed, and where the splits will happen
        // We're going to remove all CRs from the unquoted text, by normalizing all line endings to just LF
        // This ensures us that the only place CR is used, is as the escape sequences for quoted text
        return preg_replace('/\r\n?/', "\n", $matches[1]) . $str;
    }

    public static function parse_csv_line($line) {
        return array_map(array(__CLASS__, 'parse_csv_field' ), explode(',', $line));
    }

    public static function parse_csv_field($field) {
        $field = str_replace("\rC", ',', $field);
        $field = str_replace("\rQ", '"', $field);
        $field = str_replace("\rN", "\n", $field);
        $field = str_replace("\rR", "\r", $field);
        return $field;
    }

    public function analyzeText($filepath) {
        $result = [
            'text' => NULL,
            'first_name' => NULL,
            'last_name' => NULL,
            'phone' => NULL,
            'url' => NULL,
            'email' => NULL,
        ];

        $google_vision = \Drupal::service('epm.google_vision_api');
        $image_data = $google_vision->opticalCharacterRecognition($filepath);
        $result['text'] = $image_data['responses'][0]['fullTextAnnotation']['text'];
        $result_flattened = preg_replace('/\s+/', ' ', trim($result['text']));

        $tokens = preg_split("/[\s,]+/", $result['text']);
        foreach ($tokens as $token) {
            if (!$result['first_name'] && \Drupal\epm\Service\DictionaryFirstNames::isMember($token)) {
                $result['first_name'] = $token;
            }

            if ($result['first_name'] && !$result['last_name'] && \Drupal\epm\Service\DictionaryLastNames::isMember($token) && $token <> $result['first_name']) {
                $result['last_name'] = $token;
            }

            if (!$result['email'] && \Drupal::service('email.validator')->isValid($token)) {
                $result['email'] = $token;
            }

            if (!$result['url'] && filter_var($token, FILTER_VALIDATE_URL)) {
                $result['url'] = $token;
            }

            if (!$result['phone']) {
                $cleared = preg_replace('/[^0-9]/', '', $token);
                if (strlen($cleared) > 8 && strlen($cleared) < 16) {
                    $result['phone'] = $token;
                }
            }
        }

        $pattern  = '#\b(([\w-]+://?|www[.])[^\s()<>]+(?:\([\w\d]+\)|([^[:punct:]\s]|/)))#';
        preg_match($pattern, $result_flattened, $url);
        if (isset($url[0]) && $url[0]) {
            $result['url'] = $url[0];
        }

        return $result;
    }

}