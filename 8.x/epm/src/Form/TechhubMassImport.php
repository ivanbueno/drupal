<?php

namespace Drupal\epm\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\taxonomy\TermInterface;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\HtmlCommand;
use Drupal\Core\Ajax\InvokeCommand;


class TechhubMassImport extends FormBase {

    /**
     * {@inheritdoc}
     */
    public function getFormId() {
        return 'techhub_mass_import_form';
    }

    /**
     * {@inheritdoc}
     */
    public function buildForm(array $form, FormStateInterface $form_state, TermInterface $taxonomy_term = NULL) {
        $form = [];

        $form['hub'] = [
            '#type' => 'hidden',
            '#value' => $taxonomy_term->id(),
        ];
        $form['csv'] = [
            '#type' => 'textarea',
            '#title' => 'Users to be imported',
            '#description' => 'One user per line in this format: "email@acme.org", "First", "Last"',
            '#ajax' => [
                'callback' => 'Drupal\epm\Form\TechhubMassImport::previewCSV',
                'effect' => 'fade',
                'event' => 'change',
                'progress' => [
                    'type' => 'throbber',
                    'message' => NULL,
                ],
            ],
        ];
        $form['actions'] = [
            '#type' => 'actions',
            '#attributes' => [
                'class' => ['techhub-mass-import']
            ]
        ];
        $form['actions']['submit'] = [
            '#type' => 'submit',
            '#value' => $this->t('Import Users'),
        ];
        $form['actions']['submit']['#attributes']['class'][] = 'btn-primary techhub-mass-import-submit margin-top-md hidden';

        return $form;
    }

    public static function previewCSV(array &$form, FormStateInterface $form_state) {
        $ajax_response = new AjaxResponse();

        $csv_data = trim($form_state->getValue('csv'));

        if (!empty($csv_data)) {
            /* @var \Drupal\epm\Service\Techhub $techhub */
            $techhub = \Drupal::service('epm.techhub');
            $rows = $techhub::parse_csv($csv_data);
            $csv_render = [
                '#type' => 'table',
                '#header' => ['Email', 'First Name', 'Last Name'],
                '#rows' => $rows,
            ];

            $all_emails_valid = TRUE;
            foreach ($rows as $row) {
                if (!\Drupal::service('email.validator')->isValid($row[0])) {
                    $all_emails_valid = FALSE;
                }
            }

            $ajax_response->addCommand(new HtmlCommand('.techhub-mass-import-preview', $csv_render));

            if ($all_emails_valid) {
                $ajax_response->addCommand(new InvokeCommand('button.techhub-mass-import-submit', 'removeClass', ['hidden']));
                $ajax_response->addCommand(new InvokeCommand('.techhub-mass-import-warning', 'addClass', ['hidden']));
                $ajax_response->addCommand(new InvokeCommand('.techhub-mass-import-preview', 'addClass', ['bg-success']));
                $ajax_response->addCommand(new InvokeCommand('.techhub-mass-import-preview', 'removeClass', ['bg-danger']));
                $ajax_response->addCommand(new InvokeCommand('.techhub-mass-import-preview', 'removeClass', ['bg-light']));
            }
            else {
                $ajax_response->addCommand(new InvokeCommand('.techhub-mass-import-preview', 'addClass', ['bg-danger']));
                $ajax_response->addCommand(new InvokeCommand('.techhub-mass-import-preview', 'removeClass', ['bg-success']));
                $ajax_response->addCommand(new InvokeCommand('.techhub-mass-import-preview', 'removeClass', ['bg-light']));
                $ajax_response->addCommand(new InvokeCommand('button.techhub-mass-import-submit', 'addClass', ['hidden']));
                $ajax_response->addCommand(new InvokeCommand('.techhub-mass-import-warning', 'removeClass', ['hidden']));
                $ajax_response->addCommand(new HtmlCommand('.techhub-mass-import-warning', t('Some emails are invalid.  Please review and correct the issue.')));
            }
        }

        return $ajax_response;
    }

    /**
     * {@inheritdoc}
     */
    public function submitForm(array &$form, FormStateInterface $form_state) {
        /* @var \Drupal\epm\Service\Techhub $techhub */
        $techhub = \Drupal::service('epm.techhub');
        $users = $techhub::parse_csv($form_state->getValue('csv'));
        $operations = [];

        $count = 1;
        foreach ($users as $user) {
            $operations[] = [
                '\Drupal\epm\Form\TechhubMassImport::saveUser',
                [
                    $user,
                    $form_state->getValue('hub'),
                ]
            ];
            $count++;
        }

        $batch = array(
            'operations' => $operations,
            'finished' => '\Drupal\epm\Form\TechhubMassImport::saveUsersFinishedCallback',
        );
        batch_set($batch);
    }

    public static function saveUser($user, $hub, &$context){
        /* @var \Drupal\epm\Service\Techhub $techhub */
        $techhub = \Drupal::service('epm.techhub');

        $user_result = [];
        if (\Drupal::service('email.validator')->isValid($user[0])) {
            $user_result = $techhub->saveUser(
                $user[0],
                isset($user[1]) ? $user[1] : NULL,
                isset($user[2]) ? $user[2] : NULL,
                NULL,
                $hub,
                TRUE,
                FALSE
            );
        }

        // Store some result for post-processing in the finished callback.
        $context['results'][] = $user_result;

        // Optional message displayed under the progressbar.
        $context['message'] = t('Importing @user', array('@user' => $user[0]));
    }

    public static function saveUsersFinishedCallback($success, $results, $operations) {
        // The 'success' parameter means no fatal PHP errors were detected. All
        // other error management should be handled using 'results'.
        if ($success) {
            $message = \Drupal::translation()->formatPlural(
                count($results),
                'One user processed.', '@count users processed.'
            );
        }
        else {
            $message = t('Finished with an error.');
        }
        drupal_set_message($message);
    }

}
