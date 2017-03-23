;;; kill-a-lawyer.el --- hide legal verbiage at the start of programs

;;; For obvious reasons, this program has no copyright notice.

;;; Author: Ben Mesander <ben@piglet.cr.usgs.gov>
;;;         Noah Friedman <friedman@prep.ai.mit.edu>
;;; Maintainer: ben@piglet.cr.usgs.gov
;;; Keywords: extensions
;;; Created: 1994-07-24

;;; $Id: kill-a-lawyer.el,v 1.2 1994/10/28 16:47:37 ben Exp $

;;; Commentary:

;;; I'm not a lawyer, Jim, I'm a hacker.

;;; Code:

;;;###autoload
(defvar kill-a-lawyer-mode nil
  "*If non-`nil', narrow buffer to region excluding copyright message.")

;;;###autoload
(defvar kill-a-lawyer-copyright-regexp-list
  '("Code:[ 	]*$"                 ; new style GNU code
    "Cambridge, MA 02139, USA\\..*$"  ; old style GNU code, pre June '95
    "Boston, MA[ \t]+02111-1307, USA\\..*$" ; old style GNU code, post Jun '95
    "SUCH DAMAGE\.[ 	]*$"         ; BSD (mighty appropriate)
    "imitations under the licen\.*$"; Apache
    "\*\* or implied warranty\.[ 	]*$") ; NCAR
  "List of regexps that match the last line of moby copyright blocks.")

;;;###autoload
(defvar kill-a-lawyer-hook-list
  '(c-mode-hook emacs-lisp-mode-hook ksh-mode-hook)
  "List of modes to hunt down and kill legalese in.")

;;;###autoload
(defun kill-a-lawyer-mode (&optional prefix)
  "Enable or disable kill-a-lawyer-mode.
When enabled, newly-visited files which use mode hooks specified in
`kill-a-lawyer-hook-list' are narrowed to hide the copyright message
at the beginning of the buffer.  The variable
`kill-a-lawyer-copyright-regexp-list' is used to describe the end of
copyright notices, so that the narrowed region can begin there.

The command `kill-a-lawyer' will perform narrowing regardless of the mode.

A negative prefix argument disables this mode.
No argument or any non-negative argument enables it.

Users may enable or disable this mode by setting the variable of the
same name."
  (interactive "P")
  ;; make sure it's there.
  (mapcar '(lambda (hooksym)
	     (add-hook hooksym 'kill-a-lawyer-setup))
	  kill-a-lawyer-hook-list)
  (setq kill-a-lawyer-mode (>= (prefix-numeric-value prefix) 0))
  (and (interactive-p)
       (if kill-a-lawyer-mode
           (message "kill-a-lawyer-mode is enabled")
         (message "kill-a-lawyer-mode is disabled")))
  kill-a-lawyer-mode)

;;;###autoload
(defun kill-a-lawyer ()
  (interactive)
  (let ((found nil)
	(regexp-list kill-a-lawyer-copyright-regexp-list))
    (widen)
    (goto-char (point-min))
    (push-mark)
    (while (and (not found) regexp-list)
      (setq found (re-search-forward (car regexp-list) nil t))
      (setq regexp-list (cdr regexp-list))
      (and found
           (narrow-to-region found (point-max)))))
  (pop-mark))

(defun kill-a-lawyer-setup ()
  (and kill-a-lawyer-mode
       (kill-a-lawyer)))


(provide 'kill-a-lawyer)

;; This installs the setup function on find-file-hooks without actually
;; enabling the mode, so that just setting the variable will work.
(kill-a-lawyer-mode nil)

;;; kill-a-lawyer.el ends here
