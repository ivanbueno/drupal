<?php

namespace Drupal\epm\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;


class TechhubSwipeForm extends FormBase {

    /**
     * {@inheritdoc}
     */
    public function getFormId() {
        return 'techhub_swipe_form';
    }

    /**
     * {@inheritdoc}
     */
    public function buildForm(array $form, FormStateInterface $form_state, $options = array()) {
        $form = array();

        if (!$options['user']->user_picture->isEmpty()) {
            $form['photo'] = [
                '#type' => 'item',
                '#theme' => 'image_style',
                '#style_name' => 'thumbnail',
                '#uri' => $options['user']->user_picture->entity->getFileUri(),
            ];
        }

        $form['hub'] = [
            '#type' => 'radios',
            '#title' => t($options['user']->getDisplayName()),
            '#options' => $options['hubs'],
        ];
        $form['user'] = [
            '#type' => 'hidden',
            '#value' => $options['user']->id(),
        ];
        $form['actions'] = [
            '#type' => 'actions',
        ];
        $form['actions']['submit'] = [
            '#type' => 'submit',
            '#value' => $this->t('Check In'),
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
        $node = $techhub->createTicket($form_state->getValue('hub'), $form_state->getValue('user'));
        $form_state->setRedirect(
            'epm.hub',
            ['taxonomy_term' => $form_state->getValue('hub')]
        );

    }

}
