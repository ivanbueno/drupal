<?php

namespace Drupal\epm\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\taxonomy\TermInterface;


class TechhubSignUpForm extends FormBase {

    /**
     * {@inheritdoc}
     */
    public function getFormId() {
        return 'techhub_sign_up_form';
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
        $form['company'] = [
            '#type' => 'textfield',
            '#title' => 'Company',
            '#description' => 'Please enter the company',
        ];
        $form['title'] = [
            '#type' => 'textfield',
            '#title' => 'Title',
            '#description' => 'Please enter the job title',
        ];
        $form['actions'] = [
            '#type' => 'actions',
            '#attributes' => [
                'class' => ['techhub-check-in']
            ]
        ];
        $form['actions']['submit'] = [
            '#type' => 'submit',
            '#value' => $this->t('Sign Up'),
        ];
        $form['actions']['submit']['#attributes']['class'][] = 'btn-primary';

        return $form;
    }

    /**
     * {@inheritdoc}
     */
    public function submitForm(array &$form, FormStateInterface $form_state) {
        /* @var \Drupal\epm\Service\Techhub $techhub */
        $techhub = \Drupal::service('epm.techhub');
        $user = $techhub->saveUser(
            $form_state->getValue('email'),
            $form_state->getValue('first_name'),
            $form_state->getValue('last_name'),
            NULL,
            $form_state->getValue('hub'),
            FALSE,
            FALSE,
            [
                'title' => $form_state->getValue('title'),
                'company' => $form_state->getValue('company'),
            ]
        );

        $message = $techhub->prepareMail('exhibit_send_assets', [
            'to' => $form_state->getValue('email'),
            'first_name' => $form_state->getValue('first_name'),
            'last_name' => $form_state->getValue('last_name'),
            'hub' => $form_state->getValue('hub'),
        ]);
        $techhub->sendMail('default', $message);

        $node = $techhub->createTicket(
            $form_state->getValue('hub'),
            $user->id(),
            0,
            'ticket_status_complete',
            'Exhibit Sign Up');

        $form_state->setRedirect(
            'epm.hub',
            ['taxonomy_term' => $form_state->getValue('hub')]
        );
    }

}
