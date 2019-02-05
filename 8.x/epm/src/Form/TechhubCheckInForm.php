<?php

namespace Drupal\epm\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\InvokeCommand;
use Drupal\taxonomy\TermInterface;


class TechhubCheckInForm extends FormBase {

    /**
     * {@inheritdoc}
     */
    public function getFormId() {
        return 'techhub_check_in_form';
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
        $form['email'] = [
            '#type' => 'email',
            '#title' => 'Email',
            '#required' => TRUE,
            '#description' => 'Please enter an email',
            '#ajax' => [
                'callback' => 'Drupal\epm\Form\TechhubCheckinForm::userCheckExistence',
                'effect' => 'fade',
                'event' => 'change',
                'progress' => [
                    'type' => 'throbber',
                    'message' => NULL,
                ],
            ],
        ];
        $form['first_name'] = [
            '#type' => 'textfield',
            '#title' => 'First name',
            '#required' => TRUE,
            '#description' => 'Please enter the first name',
        ];
        $form['last_name'] = [
            '#type' => 'textfield',
            '#title' => 'Last name',
            '#required' => TRUE,
            '#description' => 'Please enter the last name',
        ];
        $form['actions'] = [
            '#type' => 'actions',
            '#attributes' => [
                'class' => ['techhub-check-in']
            ]
        ];
        $form['actions']['submit'] = [
            '#type' => 'submit',
            '#value' => $this->t('Check In'),
        ];
        $form['actions']['submit']['#attributes']['class'][] = 'btn-primary';

        return $form;
    }

    public static function userCheckExistence(array &$form, FormStateInterface $form_state) {
        $ajax_response = new AjaxResponse();

        if ($form_state->getValue('email') != FALSE) {
            $user = user_load_by_mail($form_state->getValue('email'));
            if ($user) {
                $ajax_response->addCommand(new InvokeCommand('input[name="first_name"]', 'val', array($user->get('field_user_first_name')->value)));
                $ajax_response->addCommand(new InvokeCommand('input[name="last_name"]', 'val', array($user->get('field_user_last_name')->value)));
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
        $user = $techhub->saveUser($form_state->getValue('email'), $form_state->getValue('first_name'), $form_state->getValue('last_name'), NULL, $form_state->getValue('hub'), TRUE, FALSE);

        $node = $techhub->createTicket($form_state->getValue('hub'), $user->id());
        $form_state->setRedirect(
            'epm.hub',
            ['taxonomy_term' => $form_state->getValue('hub')]
        );
    }

}
